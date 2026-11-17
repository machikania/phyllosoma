#include <stdio.h>
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

uint8_t TVRAM[WIDTH_XMAX*WIDTH_Y*2+1] __attribute__ ((aligned (4)));
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

uint16_t bgcolor;

struct dvi_inst dvi0;
static uint16_t rgb565_palette[PALETTE_SIZE] __attribute__((aligned(4)));
static uint32_t scanline_buf[X_RES] __attribute__((aligned(4)));
static uint32_t tmdsbuf[NUM_TMDSBUF][3 * X_RES*2 / DVI_SYMBOLS_PER_WORD];

static void __not_in_flash_func(dvi_scanbuf_palette)() {
	static uint scanline = 0;
	uint32_t *tmdsbuf;
	uint32_t *p=scanline_buf;
	uint8_t tline=scanline & 7;
	if(videomode==VMODE_WIDETEXT){
		uint8_t *tvp=TVRAM+scanline/8*WIDTH_XCL;
		for(int i=0;i<WIDTH_XCL;i++){
			uint8_t d=fontp[*tvp *8 +tline];
			uint32_t cl=rgb565_palette[*(tvp+ATTROFFSET)];
			for(int i=0;i<4;i++){
				uint32_t c;
				if(d&0x80) c=cl;
				else c=bgcolor;
				if(d&0x40) c+=cl<<16;
				else c+=bgcolor<<16;
				d<<=2;
				*p++=c;
			}
			tvp++;
		}
	}
	else if(videomode==VMODE_MONOTEXT){
		uint8_t *tvp=TVRAM+scanline/8*WIDTH_XBW;
		for(int i=0;i<WIDTH_XBW;i++){
			uint8_t d=fontp[*tvp *8 +tline];
			uint32_t cl=rgb565_palette[*(tvp+ATTROFFSETBW)];
			for(int i=0;i<4;i++){
				uint32_t c;
				if(d&0x80) c=cl;
				else c=bgcolor;
				if(d&0x40) c+=cl<<16;
				else c+=bgcolor<<16;
				d<<=2;
				*p++=c;
			}
			tvp++;
		}
	}
	else if(videomode==VMODE_WIDEGRPH){
		uint8_t *tvp=TVRAM+scanline/8*WIDTH_XCL;
	    uint8_t *gvp=GVRAM_DISP+scanline*X_RES;
		for(int i=0;i<WIDTH_XCL;i++){
			uint8_t d=fontp[*tvp *8 +tline];
			uint32_t cl=rgb565_palette[*(tvp+ATTROFFSET)];
			for(int j=0;j<4;j++){
				uint32_t c;
				if(d&0x80) c=cl;
				else c=rgb565_palette[*gvp];
				gvp++;
				if(d&0x40) c+=cl<<16;
				else c+=rgb565_palette[*gvp]<<16;
				gvp++;
				d<<=2;
				*p++=c;
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
	if(videomode==VMODE_MONOTEXT){
		tmds_encode_data_channel_fullres_16bpp(scanline_buf, tmdsbuf + 0 * X_RES, X_RES*2, DVI_16BPP_BLUE_MSB,  DVI_16BPP_BLUE_LSB );
		tmds_encode_data_channel_fullres_16bpp(scanline_buf, tmdsbuf + 1 * X_RES, X_RES*2, DVI_16BPP_GREEN_MSB, DVI_16BPP_GREEN_LSB);
		tmds_encode_data_channel_fullres_16bpp(scanline_buf, tmdsbuf + 2 * X_RES, X_RES*2, DVI_16BPP_RED_MSB,   DVI_16BPP_RED_LSB  );
	}
	else{
		tmds_encode_data_channel_16bpp(scanline_buf, tmdsbuf + 0 * X_RES, X_RES, DVI_16BPP_BLUE_MSB,  DVI_16BPP_BLUE_LSB );
		tmds_encode_data_channel_16bpp(scanline_buf, tmdsbuf + 1 * X_RES, X_RES, DVI_16BPP_GREEN_MSB, DVI_16BPP_GREEN_LSB);
		tmds_encode_data_channel_16bpp(scanline_buf, tmdsbuf + 2 * X_RES, X_RES, DVI_16BPP_RED_MSB,   DVI_16BPP_RED_LSB  );
	}
	queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);
	request_core1_callback_at(dvi_scanbuf_palette,time_us_32()+1+((scanline&3)==0));
}

void set_palette(unsigned char c,unsigned char b,unsigned char r,unsigned char g){
	rgb565_palette[c]=((r>>3)<<11) | ((g>>2)<<5) | (b>>3);
}

//バックグランドカラー設定
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g){
	bgcolor=((r>>3)<<11) | ((g>>2)<<5) | (b>>3);
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
	bgcolor=0;
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
	for(i=0;i<WIDTH_XMAX*WIDTH_Y*2/4;i++) *vp++=0;
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

//	setup_default_uart();

	pio_set_gpio_base(DVI_DEFAULT_SERIAL_CONFIG.pio,16);

	dvi0.timing = &DVI_TIMING;
	dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());
	for(int i=0;i<NUM_TMDSBUF;i++){
		void *p=&tmdsbuf[i][0];
		queue_add_blocking_u32(&dvi0.q_tmds_free, &p);
	}

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
	switch(m){
		case VMODE_WIDETEXT: // ワイドテキスト50文字モード
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				graphmode=0;
				WIDTH_X=WIDTH_XCL;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			break;
		case VMODE_MONOTEXT: // モノクロテキスト100文字モード
			if(textmode!=TMODE_MONOTEXT){
				textmode=TMODE_MONOTEXT;
				WIDTH_X=WIDTH_XBW;
				attroffset=ATTROFFSETBW;
				clearscreen();
			}
			break;
		case VMODE_WIDEGRPH: // ワイドグラフィック＋テキスト50文字モード
			GVRAM=gvram;
			GVRAM_DISP=gvram;
			graphmode=GMODE_WIDEGRPH;
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				WIDTH_X=WIDTH_XCL;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			break;
	}
	videomode=m;
}

//描画用ビデオメモリと表示用メモリの設定
void set_gvram(uint8_t *gvram_draw,uint8_t *gvram_disp){
	GVRAM=gvram_draw;
	GVRAM_DISP=gvram_disp;
}