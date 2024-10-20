#include <stdint.h>

#define X_RES	336 // Graphic横方向解像度
#define Y_RES	216 // Graphic縦方向解像度
extern int WIDTH_X; // 横方向文字数
#define WIDTH_Y 27 // 縦方向文字数
#define ATTROFFSET (WIDTH_X*WIDTH_Y) // TVRAM上のカラーパレット格納位置
#define WIDTH_XBW	80 // 横方向文字数（モノクロ時）
#define ATTROFFSETBW	(WIDTH_XBW*WIDTH_Y) // TVRAM上のカラーパレット格納位置（モノクロ時）
#define WIDTH_XMAX WIDTH_XBW // 横方向最大文字数

// videomode値
//#define VMODE_T30 0 // 標準テキスト30文字互換モード
//#define VMODE_STDTEXT 1 // 標準テキスト36文字モード
//#define VMODE_T40 2 // 標準テキスト40文字互換モード（6ドットフォント）
#define VMODE_WIDETEXT 3 // ワイドテキスト42文字モード
//#define VMODE_WIDETEXT6dot 4 // ワイドテキスト64文字モード（6ドットフォント）
#define VMODE_MONOTEXT 5 // モノクロテキスト80文字モード
//#define VMODE_ZOEAGRPH 16 // type Z互換グラフィックモード
//#define VMODE_STDGRPH 17 // 標準グラフィック＋テキスト36文字モード
#define VMODE_WIDEGRPH 18 // ワイドグラフィック＋テキスト42文字モード

// textomode値
//#define TMODE_T30 0 // 標準テキスト30文字互換モード
//#define TMODE_STDTEXT 1 // 標準テキスト36文字モード
//#define TMODE_T40 2 // 標準テキスト40文字互換モード（6ドットフォント）
#define TMODE_WIDETEXT 3 // ワイドテキスト42文字モード
//#define TMODE_WIDETEXT6dot 4 // ワイドテキスト64文字モード（6ドットフォント）
#define TMODE_MONOTEXT 5 // モノクロテキスト80文字モード

// graphmode値
#define GMODE_NOGRPH 0 // グラフィック不使用
//#define GMODE_ZOEAGRPH 1 // type Z互換グラフィックモード
//#define GMODE_STDGRPH 2 // 標準グラフィック＋テキスト36文字モード
#define GMODE_WIDEGRPH 3 // ワイドグラフィック＋テキスト42文字モード

// NTSC出力 1ラインあたりのサンプル数
#define NUM_LINE_SAMPLES 908  // 227 * 4

// NTSC出力 走査線数
#define NUM_LINES 262  // 走査線262本
#define V_SYNC		10	// 垂直同期本数
#define V_PREEQ		26	// ブランキング区間上側
#define H_SYNC		68	// 水平同期幅、約4.7μsec
#define H_PICTURE (H_SYNC+8+9*4+60) // 映像開始位置
#define init_composite rp2040_pwm_ntsc_init

// NTSC display initialization macro
#define display_init() rp2040_pwm_ntsc_init(NTSC_VIDEO_OUT)

#define VERTICAL 0
#define HORIZONTAL 1
#define LCD0TURN 0
#define LCD180TURN 2

void g_clearscreen(void);
void clearscreen(void);
void start_composite(void);
void stop_composite(void);
void rp2040_pwm_ntsc_init(uint8_t n);
void init_palette(void);
void set_videomode(unsigned char m, unsigned char *gvram); //ビデオモードの切り替え
void set_lcdalign(unsigned char align); // Dummy

extern volatile uint16_t drawcount;
extern uint8_t TVRAM[]; // Textビデオメモリ
extern uint8_t* GVRAM; // Graphicビデオメモリ
extern uint8_t videomode,textmode,graphmode; //画面モード
extern int attroffset; // TVRAMのカラー情報エリア位置
extern const uint8_t FontData[];
extern uint8_t* fontp; //現在のフォントパターンの先頭アドレス
extern uint8_t *cursor;
extern uint8_t cursorcolor;

extern int LCD_ALIGNMENT; // Dummy
