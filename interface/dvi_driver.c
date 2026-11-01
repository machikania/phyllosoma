#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "hardware/uart.h"
#include "../core1.h"

#include "../libdvi/dvi.h"
#include "../libdvi/dvi_serialiser.h"
#include "../libdvi/tmds_encode.h"
#include "../libdvi/common_dvi_pin_configs.h"
#include "dvi_driver.h"
#include "graphlib_dvi.h"

uint8_t TVRAM[WIDTH_XCL*WIDTH_Y*2+1] __attribute__ ((aligned (4)));
uint8_t *GVRAM=0; //グラフィックVRAM（描画用）開始位置のポインタ
static uint8_t *GVRAM_DISP; //グラフィックVRAM（表示用）開始位置のポインタ
uint8_t *cursor=TVRAM;
uint8_t cursorcolor=7;

volatile uint16_t drawcount=0; //　1画面表示終了ごとに1足す。アプリ側で0にする。
volatile uint8_t drawing; //　映像区間処理中は-1、その他は0
uint8_t videostop; // 1～3：ビデオ出力停止、0：ビデオ出力中

unsigned char videomode=0xff,textmode=0xff,graphmode=0xff; //画面モード
int WIDTH_X = WIDTH_XCL; // 横方向文字数
int attroffset; // TVRAMのカラー情報エリア位置
uint8_t* fontp; //現在のフォントパターンの先頭アドレス

uint8_t bgcolor=0;

struct dvi_inst dvi0;
static uint32_t tmds_palette[6 * PALETTE_SIZE] __attribute__((aligned(4)));
static uint32_t scanline_buf[X_RES*2/4] __attribute__((aligned(4)));

static void __not_in_flash_func(dvi_scanbuf_palette)() {
	static uint scanline = 0;
	uint32_t *tmdsbuf;
	uint32_t *p=scanline_buf;
	uint8_t *tvp=TVRAM+scanline/8*WIDTH_X;
	uint8_t tline=scanline & 7;
	if(videomode==VMODE_WIDETEXT){
		for(int i=0;i<WIDTH_X;i++){
			uint8_t d=fontp[*tvp *8 +tline];
			uint8_t cl=*(tvp+ATTROFFSET);
			cl+=cl<<8;
			uint32_t c;
			for(int j=0;j<4;j++){
				if(d&0x80){
					c=cl;
				}
				else{
					c=bgcolor+(bgcolor<<8);
				}
				if(d&0x40){
					c+=cl<<16;
				}
				else{
					c+=(bgcolor<<16)+(bgcolor<<24);
				}
				d<<=2;
				*p++=c;
			}
			tvp++;
		}
	}
	else if(videomode==VMODE_WIDEGRPH){
	    uint16_t *gvp=(uint16_t*)(GVRAM_DISP+scanline*X_RES);
		for(int i=0;i<WIDTH_X;i++){
			uint8_t d=fontp[*tvp *8 +tline];
			uint8_t cl=*(tvp+ATTROFFSET);
			cl+=cl<<8;
			uint32_t c;
			for(int j=0;j<4;j++){
				uint16_t g=*gvp;
				if(d&0x80){
					c=cl;
				}
				else{
					c=(uint8_t)g+((uint8_t)g<<8);
				}
				if(d&0x40){
					c+=cl<<16;
				}
				else{
					c+=((g>>8)<<16)+((g>>8)<<24);
				}
				d<<=2;
				*p++=c;
				gvp++;
			}
			tvp++;
		}
	}

	if (++scanline == Y_RES){
		scanline = 0;
		drawcount++;
		// Trigger drawcount interrupt every 1/60 second
		drawcount_interrupt();
	}
	queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);
	tmds_encode_palette_data(scanline_buf, tmds_palette, tmdsbuf, X_RES*2, PALETTE_BITS);
	queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);
	request_core1_callback_at(dvi_scanbuf_palette,time_us_32()+2);
}

static const int8_t imbalance_lookup[16] = { -4, -2, -2, 0, -2, 0, 0, 2, -2, 0, 0, 2, 0, 2, 2, 4 };

static inline int byte_imbalance(uint32_t x) {
	return imbalance_lookup[x >> 4] + imbalance_lookup[x & 0xF];
}

static void tmds_encode_symbols(uint8_t pixel, uint32_t* negative_balance_sym, uint32_t* positive_balance_sym) {
	int pixel_imbalance = byte_imbalance(pixel);
	uint32_t sym = pixel & 1;
	if (pixel_imbalance > 0 || (pixel_imbalance == 0 && sym == 0)) {
		for (int i = 0; i < 7; ++i) {
			sym |= (~((sym >> i) ^ (pixel >> (i + 1))) & 1) << (i + 1);
		}
	}
	else {
		for (int i = 0; i < 7; ++i) {
			sym |= ( ((sym >> i) ^ (pixel >> (i + 1))) & 1) << (i + 1);
		}
		sym |= 0x100;
	}

	int imbalance = byte_imbalance(sym & 0xFF);
	if (imbalance == 0) {
		if ((sym & 0x100) == 0) sym ^= 0x2ff;
		*positive_balance_sym = sym;
		*negative_balance_sym = sym;
		return;
	}
	else if (imbalance > 0) {
		*negative_balance_sym = (sym ^ 0x2ff) | (((-imbalance + imbalance_lookup[2 ^ (sym >> 8)] + 2) & 0x3F) << 26);
		*positive_balance_sym = sym | ((imbalance + imbalance_lookup[sym >> 8] + 2) << 26);
	}
	else {
		*negative_balance_sym = sym | (((imbalance + imbalance_lookup[sym >> 8] + 2) & 0x3F) << 26);
		*positive_balance_sym = (sym ^ 0x2ff) | ((-imbalance + imbalance_lookup[2 ^ (sym >> 8)] + 2) << 26);
	}
}

void set_palette(unsigned char c,unsigned char b,unsigned char r,unsigned char g){
	uint32_t* tmds_palette_blue = tmds_palette;
	uint32_t* tmds_palette_green = tmds_palette + 2 * PALETTE_SIZE;
	uint32_t* tmds_palette_red = tmds_palette + 4 * PALETTE_SIZE;
	tmds_encode_symbols(b, &tmds_palette_blue[c], &tmds_palette_blue[c + PALETTE_SIZE]);
	tmds_encode_symbols(g, &tmds_palette_green[c], &tmds_palette_green[c + PALETTE_SIZE]);
	tmds_encode_symbols(r, &tmds_palette_red[c], &tmds_palette_red[c + PALETTE_SIZE]);
}

//バックグランドカラー設定（仮実装）Provisional implementation
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g){
	bgcolor=255;
	set_palette(255,b,r,g);
}

void init_palette(void){
	//カラーパレット初期化
	int i;
	for(i=0;i<8;i++){
		set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
	}
	for(i=0;i<8;i++){
		set_palette(i+8,128*(i&1),128*((i>>1)&1),128*(i>>2));
	}
	for(i=16;i<256;i++){
		set_palette(i,255,255,255);
	}
}

// グラフィック画面クリア
void g_clearscreen(void)
{
	// グラフィックメモリが確保されていない場合メッセージを表示
	if(GVRAM==0){
		uint8_t message[]="Not allocated graphic memory";
		uint8_t *p=TVRAM, *s=message;
		while(*s){
			*(p+attroffset)=7;
			*p++=*s++;
		}
		return;
	}
	unsigned int *vp=(unsigned int *)GVRAM;
	for(int i=0;i<X_RES*Y_RES/4;i++) *vp++=0;
}

//テキスト画面クリア
void clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)TVRAM;
	for(i=0;i<WIDTH_XCL*WIDTH_Y*2/4;i++) *vp++=0;
	cursor=TVRAM;
}

//ビデオ出力開始
//未実装 Not yet implemented
void start_composite(void)
{
	videostop=0;
}

//ビデオ出力停止
//未実装 Not yet implemented
void stop_composite(void)
{
	videostop=1;
}

//ビデオ出力システムを初期化し開始する
void init_dvi_driver(void){
	//ビデオモード初期化
	set_videomode(VMODE_WIDETEXT,0);
	init_palette();
	fontp=(uint8_t*)FontData; //標準フォントに設定

	vreg_set_voltage(VREG_VSEL);
	sleep_ms(10);
	set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);

	setup_default_uart();

	pio_set_gpio_base(DVI_DEFAULT_SERIAL_CONFIG.pio,16);

	dvi0.timing = &DVI_TIMING;
	dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

    clearscreen();
    init_palette();

	//DMA割り込み優先度をタイマー割り込みより高くする
	irq_set_priority(DMA_IRQ_0, 0x40);
	//irq_set_priority(TIMER_ALARM_IRQ_NUM(alarm_pool_get_default_timer(),
	//	PICO_TIME_DEFAULT_ALARM_POOL_HARDWARE_ALARM_NUM),0xC0);

	dvi_register_irqs_this_core(&dvi0, DMA_IRQ_0);
	dvi_start(&dvi0);
	start_core1();
	videostop=0;
	request_core1_callback(dvi_scanbuf_palette);
}

//ビデオモードの切り替え
// m:ビデオモード
// gvram:グラフィック用メモリ先頭アドレス
void set_videomode(unsigned char m, unsigned char *gvram){
	if(videomode==m) return;
//	stop_composite();
	switch(m){
		case VMODE_STDTEXT: // 標準テキスト40文字モード（未実装）
		case VMODE_WIDETEXT: // ワイドテキストモード
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				graphmode=0;
				WIDTH_X=WIDTH_XCL;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			videomode=VMODE_WIDETEXT;
			break;
		case VMODE_STDGRPH: // 標準グラフィック＋テキスト40文字モード（未実装）
		case VMODE_WIDEGRPH: // ワイドグラフィック＋テキストモード
			GVRAM=gvram;
			GVRAM_DISP=gvram;
			graphmode=GMODE_WIDEGRPH;
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				WIDTH_X=WIDTH_XCL;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			videomode=VMODE_WIDEGRPH;
			break;
	}
//	start_composite();
}

//描画用ビデオメモリと表示用メモリの設定
void set_gvram(uint8_t *gvram_draw,uint8_t *gvram_disp){
	GVRAM=gvram_draw;
	GVRAM_DISP=gvram_disp;
}