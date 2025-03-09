/*----------------------------------------------------------------------------

Copyright (C) 2024, KenKen, all right reserved.

This program supplied herewith by KenKen is free software; you can
redistribute it and/or modify it under the terms of the same license written
here and only for non-commercial purpose.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of FITNESS FOR A PARTICULAR
PURPOSE. The copyright owner and contributors are NOT LIABLE for any damages
caused by using this program.

----------------------------------------------------------------------------*/

// This signal generation program (using PWM and DMA) is the idea of @lovyan03.
// https://github.com/lovyan03/

//#pragma GCC optimize ("O3")

#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "rp2040_pwm_ntsc_textgraph.h"

// デバッグ用、割込み処理中HIGHになるピン
//#define PIN_DEBUG_BUSY 15

uint8_t TVRAM[WIDTH_XMAX*WIDTH_Y*2+1];
uint8_t *GVRAM=0; //グラフィックVRAM開始位置のポインタ
uint8_t *cursor=TVRAM;
uint8_t cursorcolor=7;

volatile static uint16_t scanline;
volatile uint16_t drawcount=0; //　1画面表示終了ごとに1足す。アプリ側で0にする。
volatile uint8_t drawing; //　映像区間処理中は-1、その他は0
uint8_t videostop; // 1～3：ビデオ出力停止、0：ビデオ出力中

unsigned char videomode=0xff,textmode=0xff,graphmode=0xff; //画面モード
int WIDTH_X = 42; // 横方向文字数
int attroffset; // TVRAMのカラー情報エリア位置
uint8_t* fontp; //現在のフォントパターンの先頭アドレス

static uint16_t black_level=2, white_level=9;
static uint8_t ntsc_speed=2; //1:通常 2:倍速

// DMAピンポンバッファ
uint16_t dma_buffer[2][NUM_LINE_SAMPLES] __attribute__ ((aligned (4)));

// カラーパレット
uint16_t color_tbl[4*257] __attribute__ ((aligned (4)));

static uint pwm_dma_chan0,pwm_dma_chan1;

static void __not_in_flash_func() makeDmaBuffer(uint16_t* buf, size_t line_num)
{
	uint8_t* fbp;
	uint8_t* tvp;
	uint8_t tline = 0;
	uint16_t* b = buf;

	if(videostop){
		if(videostop<3){
			drawing=0;
			while (b < buf + NUM_LINE_SAMPLES) *b++ = 0;
			videostop++;
		}
		if(line_num==V_SYNC+V_PREEQ+Y_RES){
			drawcount++;
		}
		return;
	}
	if (line_num < 2)
	{
		//垂直同期信号生成
		for (int j = 0; j < NUM_LINE_SAMPLES-H_SYNC; j++) *b++ = 0;
		while (b < buf + NUM_LINE_SAMPLES) *b++ = black_level;
	}
	if(videomode==VMODE_MONOTEXT){
		//モノクロテキストモード
		if(line_num==V_SYNC || line_num==V_SYNC+1)
		{
			//水平同期
			for (int j = 0; j < H_SYNC; j++) *b++ = 0;
			while (b < buf + NUM_LINE_SAMPLES) *b++ = black_level;
		}
		else if(line_num>=V_SYNC+V_PREEQ && line_num<V_SYNC+V_PREEQ+Y_RES)
		{
			//映像領域信号生成
			b+=H_PICTURE;
			if (line_num == V_SYNC + V_PREEQ)
			{
				drawing = -1;
			}
			tvp = TVRAM+((line_num-(V_SYNC + V_PREEQ))>>3)*WIDTH_XBW;
			tline = (line_num-(V_SYNC + V_PREEQ)) & 7;
			for(int i=0;i<WIDTH_XBW;i++)
			{
				uint8_t d=fontp[*tvp *8 +tline];
				if(*(tvp+ATTROFFSETBW)&0x80) d=~d; // カラー0x80以上の場合反転
				for(int j=0;j<8;j++)
				{
					if(d & 0x80){
						*b=white_level; //white level
					}
					else{
						*b=black_level; //black level
					}
					b++;
					d<<=1;
				}
				tvp++;
			}
		}
	}
	else{
		//カラーモード
		if(line_num==V_SYNC || line_num==V_SYNC+1)
		{
			//水平同期＋バースト信号生成
			for (int j = 0; j < H_SYNC; j++) *b++ = 0;
			for (int j = 0; j < 8; j++) *b++ = black_level;
			for (int j = 0; j < 9; j++)
			{
				*b++=black_level;
				*b++=black_level-ntsc_speed;
				*b++=black_level;
				*b++=black_level+ntsc_speed;
			}
			while (b < buf + NUM_LINE_SAMPLES) *b++ = black_level;
		}
		else if(line_num>=V_SYNC+V_PREEQ && line_num<V_SYNC+V_PREEQ+Y_RES)
		{
			//映像領域信号生成
			b+=H_PICTURE;
			if (line_num == V_SYNC + V_PREEQ)
			{
				drawing = -1;
			}
			tvp = TVRAM+((line_num-(V_SYNC + V_PREEQ))>>3)*WIDTH_XCL;
			tline = (line_num-(V_SYNC + V_PREEQ)) & 7;
			if(videomode==VMODE_WIDETEXT){
				//テキストモード
				uint32_t bc1=*((uint32_t*)(color_tbl+256*4));
				uint32_t bc2=*((uint32_t*)(color_tbl+256*4+2));
				for(int i=0;i<WIDTH_XCL;i++)
				{
					uint8_t d=fontp[*tvp *8 +tline];
					uint16_t* clp=color_tbl+(*(tvp+ATTROFFSET))*4;
					uint32_t c1=*((uint32_t*)clp);
					uint32_t c2=*((uint32_t*)(clp+2));
					for(int j=0;j<4;j++)
					{
						if(d & 0x80){
							*((uint32_t*)b)=c1;
						}
						else{
							*((uint32_t*)b)=bc1;
						}
						b+=2;
						if(d & 0x40){
							*((uint32_t*)b)=c2;
						}
						else{
							*((uint32_t*)b)=bc2;
						}
						b+=2;
						d<<=2;
					}
					tvp++;
				}
			}
			else if(videomode==VMODE_WIDEGRPH){
				fbp = GVRAM+(line_num-(V_SYNC + V_PREEQ))*X_RES;
				//グラフィック＋テキストモード
				for(int i=0;i<WIDTH_XCL;i++)
				{
					uint8_t d=fontp[*tvp *8 +tline];
					uint16_t* clp=color_tbl+(*(tvp+ATTROFFSET))*4;
					uint32_t c1=*((uint32_t*)clp);
					uint32_t c2=*((uint32_t*)(clp+2));
					for(int j=0;j<4;j++)
					{
						uint16_t t=*(uint16_t*)fbp;
						if(d & 0x80){
							*((uint32_t*)b)=c1;
						}
						else{
							*((uint32_t*)b)=*((uint32_t*)(color_tbl+(t & 0xff)*4));
						}
						b+=2;
						if(d & 0x40){
							*((uint32_t*)b)=c2;
						}
						else{
							*((uint32_t*)b)=*((uint32_t*)(color_tbl+(t >> 8)*4+2));
						}
						b+=2;
						fbp+=2;
						d<<=2;
					}
					tvp++;
				}
			}
		}
	}
	if(line_num==V_SYNC+V_PREEQ+Y_RES || line_num==V_SYNC+V_PREEQ+Y_RES+1)
	{
		if(line_num==V_SYNC+V_PREEQ+Y_RES){
			drawing=0;
			drawcount++;
		}
		//映像領域信号消去
		b+=H_PICTURE;
		for(int i=0;i<X_RES*2;i++) *b++ = black_level;
	}
}

void set_palette_main(unsigned short c,unsigned char b,unsigned char r,unsigned char g)
{
	// カラーパレット設定
	// c:パレット番号0-255、r,g,b:0-255
	// cが256のときバックグランドカラー
	if (256<c) return;

	// 輝度Y=0.587*G+0.114*B+0.299*R
	// 信号N=Y+0.4921*(B-Y)*sinθ+0.8773*(R-Y)*cosθ
	// 出力データS=N*(white-black)/255+black  white=9,black=2

	int32_t y,s;
	y=(150*g+29*b+77*r+128)/256;

	// 微妙な色ズレを修正
/*
	float th=30.0/180*3.14159;
	int32_t b_y_1=(int32_t)((b-y)*256*7*0.4921*sinf(0+th));
	int32_t r_y_1=(int32_t)((r-y)*256*7*0.8773*cosf(0+th));
	int32_t b_y_2=(int32_t)((b-y)*256*7*0.4921*sinf(3.14159/2+th));
	int32_t r_y_2=(int32_t)((r-y)*256*7*0.8773*cosf(3.14159/2+th));
*/
	int32_t b_y_1=(b-y)*441;
	int32_t r_y_1=(r-y)*1361;
	int32_t b_y_2=(b-y)*764;
	int32_t r_y_2=(r-y)*(-786);

	s=(y*1792 + b_y_1 + r_y_1 + 2*65536+32768)*ntsc_speed/65536;
	color_tbl[c*4] = s<0 ? 0 : s;
	s=(y*1792 + b_y_2 + r_y_2 + 2*65536+32768)*ntsc_speed/65536;
	color_tbl[c*4+1] = s<0 ? 0 : s;
	s=(y*1792 - b_y_1 - r_y_1 + 2*65536+32768)*ntsc_speed/65536;
	color_tbl[c*4+2] = s<0 ? 0 : s;
	s=(y*1792 - b_y_2 - r_y_2 + 2*65536+32768)*ntsc_speed/65536;
	color_tbl[c*4+3] = s<0 ? 0 : s;
}

void set_bgcolor(unsigned char b,unsigned char r,unsigned char g){
	//バックグランドカラー設定
	set_palette_main(256,b,r,g);
}

void set_palette(unsigned char c,unsigned char b,unsigned char r,unsigned char g){
	set_palette_main((unsigned short)c,b,r,g);
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
	set_bgcolor(0,0,0); //バックグランドカラー設定
}

static void irq_handler(void) {
	volatile uint32_t s0;
	uint16_t dc=drawcount;

#if defined ( PIN_DEBUG_BUSY )
	gpio_put(PIN_DEBUG_BUSY, 1);
#endif
	s0=dma_hw->ints0;
	dma_hw->ints0 = s0;
	if(s0 & (1u << pwm_dma_chan1)){
		makeDmaBuffer(dma_buffer[1], scanline);
		dma_channel_set_read_addr(pwm_dma_chan1, dma_buffer[1], false);
	}
	else{
		makeDmaBuffer(dma_buffer[0], scanline);
		dma_channel_set_read_addr(pwm_dma_chan0, dma_buffer[0], false);
	}
	if (++scanline >= NUM_LINES) {
		scanline = 0;
	}
	// Trigger drawcount interrupt every 1/60 second
	if (dc!=drawcount) drawcount_interrupt();
#if defined ( PIN_DEBUG_BUSY )
	gpio_put(PIN_DEBUG_BUSY, 0);
#endif
}

// グラフィック画面クリア
void g_clearscreen(void)
{
	// グラフィックメモリが確保されていない場合メッセージを表示して停止
	if(GVRAM==0){
		uint8_t message[]="Not allocated graphic memory";
		uint8_t *p=TVRAM, *s=message;
		while(*s){
			*(p+attroffset)=7;
			*p++=*s++;
		}
		while(true) asm("wfi");
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

void set_dma_channel_chain(uint ch,uint ch_to){
	uint32_t a;
	a=dma_channel_hw_addr(ch)->al1_ctrl;
	a=(a & ~DMA_CH0_CTRL_TRIG_CHAIN_TO_BITS) | (ch_to << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB);
	dma_channel_hw_addr(ch)->al1_ctrl=a;
}
//ビデオ出力開始
void start_composite(void)
{
	scanline=0;
	videostop=0;
/*
	makeDmaBuffer(dma_buffer[0], 0);
	makeDmaBuffer(dma_buffer[1], 1);
	dma_set_irq0_channel_mask_enabled((1u<<pwm_dma_chan0)|(1u<<pwm_dma_chan1),true);
	set_dma_channel_chain(pwm_dma_chan0, pwm_dma_chan1);
	set_dma_channel_chain(pwm_dma_chan1, pwm_dma_chan0);
	dma_channel_start(pwm_dma_chan0);
*/
}
//ビデオ出力停止
void stop_composite(void)
{
	videostop=1;
/*
	dma_set_irq0_channel_mask_enabled((1u<<pwm_dma_chan0)|(1u<<pwm_dma_chan1),false);
	set_dma_channel_chain(pwm_dma_chan0, pwm_dma_chan0);
	set_dma_channel_chain(pwm_dma_chan1, pwm_dma_chan1);
	dma_channel_abort(pwm_dma_chan0);
	dma_channel_abort(pwm_dma_chan1);
*/
}

//ビデオ出力システムを初期化し開始する
//n:信号出力するGPIO番号
void rp2040_pwm_ntsc_init(uint8_t n)
{
#if defined ( PIN_DEBUG_BUSY )
	gpio_init(PIN_DEBUG_BUSY);
	gpio_set_dir(PIN_DEBUG_BUSY, GPIO_OUT);
#endif
	//ビデオモード初期化
	set_videomode(VMODE_WIDETEXT,0);
	init_palette();
	fontp=(uint8_t*)FontData; //標準フォントに設定

	// CPUを157.5MHzで動作させる
	uint32_t freq_khz = 157500*ntsc_speed;

	// PWM周期を11サイクルとする (157.5 [MHz] / 11 = 14318181 [Hz])
	uint32_t pwm_div = 11*ntsc_speed;

	if(ntsc_speed==2){
		black_level=5;
		white_level=18;
		vreg_set_voltage(15);
	}

	// ※ NTSCのカラー信号を1周期4サンプルで出力する。
	// 出力されるカラーバースト信号は  14318181 [Hz] / 4 = 3579545 [Hz] となる。

	set_sys_clock_khz(freq_khz, true);

	gpio_set_function(n, GPIO_FUNC_PWM);
	uint pwm_slice_num = pwm_gpio_to_slice_num(n);

	pwm_config config = pwm_get_default_config();
	pwm_config_set_clkdiv(&config, 1);

	pwm_init(pwm_slice_num, &config, true);
	pwm_set_wrap(pwm_slice_num, pwm_div - 1);

	volatile void* wr_addr = &pwm_hw->slice[pwm_slice_num].cc;
	wr_addr = (volatile void*)(((uintptr_t)wr_addr) + 2);

	pwm_dma_chan0 = dma_claim_unused_channel(true);
	pwm_dma_chan1 = dma_claim_unused_channel(true);

	dma_channel_config c = dma_channel_get_default_config(pwm_dma_chan0);
	channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
	channel_config_set_read_increment(&c, true);
	channel_config_set_write_increment(&c, false);
	channel_config_set_dreq(&c, DREQ_PWM_WRAP0 + pwm_slice_num);
	channel_config_set_chain_to(&c, pwm_dma_chan1);
	dma_channel_configure(
		pwm_dma_chan0,
		&c,
		wr_addr,
		dma_buffer[0],
		NUM_LINE_SAMPLES,
		false
	);

	c = dma_channel_get_default_config(pwm_dma_chan1);
	channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
	channel_config_set_read_increment(&c, true);
	channel_config_set_write_increment(&c, false);
	channel_config_set_dreq(&c, DREQ_PWM_WRAP0 + pwm_slice_num);
	channel_config_set_chain_to(&c, pwm_dma_chan0);
	dma_channel_configure(
		pwm_dma_chan1,
		&c,
		wr_addr,
		dma_buffer[1],
		NUM_LINE_SAMPLES,
		false
	);
	scanline=0;
	videostop=0;
	makeDmaBuffer(dma_buffer[0], 0);
	makeDmaBuffer(dma_buffer[1], 1);
	dma_set_irq0_channel_mask_enabled((1u<<pwm_dma_chan0)|(1u<<pwm_dma_chan1),true);
	irq_set_exclusive_handler(DMA_IRQ_0, irq_handler);
	irq_set_enabled(DMA_IRQ_0, true);
	dma_channel_start(pwm_dma_chan0);
}

//ビデオモードの切り替え
// m:ビデオモード
// gvram:グラフィック用メモリ先頭アドレス
void set_videomode(unsigned char m, unsigned char *gvram){

	unsigned int *fontROMp,*fontRAMp;
	unsigned int i;

	if(videomode==m) return;
//	stop_composite();
	switch(m){
		case VMODE_WIDETEXT: // ワイドテキスト48文字モード
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				graphmode=0;
				WIDTH_X=WIDTH_XCL;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			break;
		case VMODE_MONOTEXT: // モノクロテキスト80文字モード
			if(textmode!=TMODE_MONOTEXT){
				textmode=TMODE_MONOTEXT;
				WIDTH_X=WIDTH_XBW;
				attroffset=ATTROFFSETBW;
				clearscreen();
			}
			break;
		case VMODE_WIDEGRPH: // ワイドグラフィック＋テキスト42文字モード
			GVRAM=gvram;
			g_clearscreen();
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
//	start_composite();
}
