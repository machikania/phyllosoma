void clearscreen(void); //チE��スト画面クリア
void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g); //チE��ストパレチE��設宁E
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g); //バックグランドカラー設宁E
void init_textgraph(unsigned char align); //LCDチE��スト�EグラフィチE��機�E利用準備

void textredraw(void);
	// チE��スト画面再描画、テキスチERAMの冁E��にしたがって液晶に出劁E
void vramscroll(void);
	//1行スクロール
void vramscrolldown(void);
	//1行送E��クロール
void setcursor(unsigned char x,unsigned char y,unsigned char c);
	//カーソル位置とカラーを設宁E
void setcursorcolor(unsigned char c);
	//カーソル位置そ�Eままでカラー番号をcに設宁E
void printchar(unsigned char n);
	//カーソル位置にチE��ストコードnめE斁E��表示し、カーソルめE斁E��進める
void printstr(unsigned char *s);
	//カーソル位置に斁E���Esを表示
void printnum(unsigned int n);
	//カーソル位置に符号なし整数nめE0進数表示
void printnum2(unsigned int n,unsigned char e);
	//カーソル位置に符号なし整数nをe桁�E10進数表示�E�前の空き桁E��刁E�Eスペ�Eスで埋める！E
void cls(void);
	//チE��スト画面消去し、カーソルを�E頭に移勁E
void startPCG(unsigned char *p,int a);
	// RAMフォント！ECG�E��E利用開始、pがフォント格納場所、aぁE以外でシスチE��フォントをコピ�E
void stopPCG(void);
	// RAMフォント！ECG�E��E利用停止


void g_pset(int x,int y,unsigned char c);
// (x,y)の位置にカラーcで点を描画

void g_putbmpmn(int x,int y,unsigned char m,unsigned char n,const unsigned char bmp[]);
// 横m*縦nドット�Eキャラクターを雍Wx,yに表示
// unsigned char bmp[m*n]配�Eに、単純にカラー番号を髞�E�る
// カラー番号ぁEの部刁E�E透�E色として扱ぁE

void g_clrbmpmn(int x,int y,unsigned char m,unsigned char n);
// 縦m*横nドット�Eキャラクター習雕
// カラー0で塗りつぶぁE

void g_gline(int x1,int y1,int x2,int y2,unsigned char c);
// (x1,y1)-(x2,y2)にカラーcで線�Eを描画

void g_hline(int x1,int x2,int y,unsigned char c);
// (x1,y)-(x2,y)への水平ラインを高速描画

void g_circle(int x0,int y0,unsigned int r,unsigned char c);
// (x0,y0)を中忁E��、半径r、カラーcの冁E��描画

void g_boxfill(int x1,int y1,int x2,int y2,unsigned char c);
// (x1,y1),(x2,y2)を対角線とするカラーcで塗られた長方形を描画

void g_circlefill(int x0,int y0,unsigned int r,unsigned char c);
// (x0,y0)を中忁E��、半径r、カラーcで塗られた冁E��描画

void g_putfont(int x,int y,unsigned char c,int bc,unsigned char n);
//8*8ドット�Eアルファベットフォント表示
//座標！E,y)、カラー番号c
//bc:バックグランドカラー、負数の場合無要E
//n:斁E��番号

void g_printstr(int x,int y,unsigned char c,int bc,unsigned char *s);
//座樁Ex,y)からカラー番号cで斁E���Esを表示、bc:バックグランドカラー

void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n);
//座樁Ex,y)にカラー番号cで数値nを表示、bc:バックグランドカラー

void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e);
//座樁Ex,y)にカラー番号cで数値nを表示、bc:バックグランドカラー、e桁で表示

unsigned int g_color(int x,int y);
//座樁Ex,y)の色を返す�E�EGB565形式！E

void g_clearscreen(void);
// グラフィチE��画面クリア

void set_lcdalign(unsigned char align);
// 液晶の縦横設宁E

extern unsigned short palette[];
//パレチE��用配�E

extern const unsigned char FontData[];
//フォントデータ

extern unsigned char TVRAM[];
//チE��スチERAM

extern int WIDTH_X; // 横方向文字数
extern int WIDTH_Y; // 縦方向文字数
#define ATTROFFSET (LCD_COLUMN_RES*LCD_ROW_RES/64) // VRAM上�EカラーパレチE��格納位置
