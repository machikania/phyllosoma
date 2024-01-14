/*----------------------------------------------------------------------------

Copyright (C) 2023, KenKen, all right reserved.

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

#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "./editor.h"
#include "./keyinput.h"
#include "./compiler.h"
#include "./config.h"
#include "./interface/usbkeyboard.h"
#include "./interface/graphlib.h"
#include "./interface/LCDdriver.h"
#include "./interface/ff.h"

//リンク付きのテキストバッファ
struct _TBUF{
	struct _TBUF *prev;//前方へのリンク。NULLの場合先頭または空き
	struct _TBUF *next;//後方へのリンク。NULLの場合最後
	unsigned short n;//現在の使用バイト数
	unsigned char Buf[TBUFSIZE];//バッファ
} ;
typedef struct _TBUF _tbuf;

//_tbuf TextBuffer[TBUFMAXLINE]; //テキストバッファ
static _tbuf *TextBuffer; //実体は配列RAM[]の中に確保する

static _tbuf *TBufstart; //テキストバッファの先頭位置
static _tbuf *cursorbp; //現在のカーソル位置のテキストバッファ
static unsigned short cursorix; //現在のカーソル位置のテキストバッファ先頭からの位置
static _tbuf *disptopbp; //現在表示中画面左上のテキストバッファ
static unsigned short disptopix; //現在表示中画面左上のテキストバッファ先頭からの位置
static int num; //現在バッファ内に格納されている文字数
static int cx,cy; //カーソル座標
static int cx2; //上下移動時の仮カーソルX座標
static _tbuf *cursorbp1; //範囲選択時のカーソルスタート位置のテキストバッファ、範囲選択モードでない場合NULL
static unsigned short cursorix1; //範囲選択時のカーソルスタート位置のテキストバッファ先頭からの位置
static int cx1,cy1; //範囲選択時のカーソルスタート座標
static int line_no; //現在のカーソル位置の行
static int line_no1; //範囲選択時のカーソルスタート位置の行
static int EDITWIDTHY; //エディタ画面の行数

// カーソル関連位置の一時避難用
static _tbuf *cursorbp_t;
static unsigned short cursorix_t;
static _tbuf *disptopbp_t;
static unsigned short disptopix_t;
static int cx_t,cy_t,line_no_t;

//unsigned char clipboard[WIDTH_XMAX*EDITWIDTHY]; //クリップボード、最大サイズは編集画面領域と同じ
static unsigned char *clipboard; //実体は配列RAM[]の中に確保する

static int clipsize; //現在クリップボードに格納されている文字数
static int edited; //保存後に変更されたかを表すフラグ

//各種配列用確保メモリ
static unsigned char *EDITORRAM=(unsigned char *)kmbasic_object;

//配列EDITORRAM[]内にメモリ動的確保するためのポインタ
static unsigned char *editormallocp;

//unsigned char filebuf[FILEBUFSIZE]; //ファイルアクセス用バッファ
static unsigned char *filebuf; //実体は配列EDITORRAM[]の中に確保する

//unsigned char cwdpath[PATHNAMEMAX]; //現在のディレクトリのパス名
static unsigned char *cwdpath; //実体は配列EDITORRAM[]の中に確保する

//unsigned char temppath[PATHNAMEMAX]; //テンポラリのパス名
static unsigned char *temppath; //実体は配列EDITORRAM[]の中に確保する

static unsigned char currentfile[13],tempfile[13]; //編集中のファイル名、一時ファイル名

//FILINFO files[MAXFILENUM]; //ロード時のファイル名一覧バッファ
static FILINFO *files; //実体は配列EDITORRAM[]の中に確保する

//unsigned char undobuf[UNDOBUFSIZE]; //アンドゥ用バッファ
static unsigned char *undobuf; //実体は配列EDITORRAM[]の中に確保する
static unsigned char *undobuf_top; //アンドゥ用バッファの先頭を指すポインタ
static int undobuf_used; //アンドゥ用バッファ使用量

static const unsigned char Message1[]="Hit Any Key\n";
static const unsigned char Message2[]="File System Error\n";
static const unsigned char Message3[]="Retry:[Enter] / Quit:[ESC]\n";
static const unsigned char ROOTDIR[]="/";

//配列EDITORRAM[]内にサイズsizeの領域を確保し、先頭アドレスを返す
//確保できない場合は、エラー表示し動作停止
unsigned char * editormalloc(int size){
	unsigned char *p;
	if(editormallocp+size>EDITORRAM+EDITORRAMSIZE){
		printstr("Cannot allocate memory");
		while(1) ;
	}
	p=editormallocp;
	editormallocp+=size;
	return p;
}

// テキストバッファ上の位置からテキスト全体の先頭から何文字目かを返す
// bp:テキストバッファポインタ
// ix:bp->Bufの先頭からの文字数
unsigned int bpixtopos(_tbuf *bp,unsigned int ix){
	unsigned int pos;
	_tbuf *sbp;
	pos=0;
	sbp=TBufstart;
	while(sbp!=bp){
		pos+=sbp->n;
		sbp=sbp->next;
		if(sbp==NULL) return 0; //エラー
	}
	return pos+ix;
}

// テキスト全体の先頭からpos文字目のテキストバッファ上の位置を返す
// 戻り値　テキストバッファポインタ
// *pix（戻り値）：戻り値テキストバッファの先頭からの位置（ポインタ渡し）
_tbuf * postobpix(int pos,unsigned short *pix){
	_tbuf *bp;
	bp=TBufstart;
	while(pos >= bp->n){
		if(bp->next==NULL) break; //全体最後尾の場合
		pos-=bp->n;
		bp=bp->next;
	}
	if(pos > bp->n){
		// オーバーランエラーの場合先頭を返す
		*pix=0;
		return TBufstart;
	}
	*pix=pos;
	return bp;
}

// テキスト全体の先頭からline行目のテキストバッファ上の位置を返す
// 戻り値　テキストバッファポインタ、line不正の場合NULL
// *pix（戻り値）：戻り値テキストバッファの先頭からの位置（ポインタ渡し）
_tbuf * linetobpix(int line,unsigned short *pix){
	_tbuf *bp,*bp2;
	int ix,ix2;
	bp=TBufstart;
	bp2=TBufstart;
	ix=0;
	ix2=0;
	if(line<1) return NULL;
	while(line>1){
		while(1){
			if(ix>=bp->n){
				if(bp->next==NULL) return NULL;
				bp=bp->next;
				ix=0;
				continue;
			}
			if(bp->Buf[ix++] == '\n'){
				bp2=bp;
				ix2=ix;
				break;
			}
		}
		line--;
	}
	*pix=ix2;
	return bp2;
}

// 新しいテキストバッファ1行を生成
// prev:挿入先の行（prevの後ろに追加）
// 戻り値　生成したバッファへのポインタ、生成できない場合NULL
_tbuf * newTBuf(_tbuf *prev){
	_tbuf *bp,*next;

	//バッファの先頭から空きをサーチ
	bp=TextBuffer;
	while(1){
		if(bp->prev==NULL && bp!=TBufstart) break;
		bp++;
		if(bp>=TextBuffer+TBUFMAXLINE) return NULL;//最後まで空きなし
	}
	next=prev->next;
	//行挿入
	bp->prev=prev;
	bp->next=next;
	prev->next=bp;
	if(next!=NULL) next->prev=bp;
	bp->n=0;
	return bp;
}

// テキストバッファの削除
// bp:削除する行のポインタ
// 戻り値　削除前の次のバッファへのポインタ、ない場合NULL
_tbuf * deleteTBuf(_tbuf *bp){
	unsigned short a,b;
	_tbuf *prev,*next;
	prev=bp->prev;
	next=bp->next;
	if(prev==NULL){
		//先頭行の場合
		if(next==NULL) return next; //最後の1行の場合は削除しない
		TBufstart=next; //次の行を先頭行設定
	}
	else prev->next=next; //前を次にリンク（最終行ならNULLがコピーされる）
	if(next!=NULL) next->prev=prev; //次があれば次を前にリンク
	bp->prev=NULL; //空きフラグ設定
	return next;
}

// アンドゥバッファ
/*
UNDOBUFSIZEバイトの環状バッファ。テキストバッファに対する変更発生ごとに、
変更内容、変更場所をバッファの先頭に記録し、先頭位置を進める。
アンドゥ実行が呼び出されると、バッファ先頭から読み出し、テキストバッファに対して
元に戻す変更を行う。
バッファがいっぱいになると、最後尾から消去（上書き）していく。

＜バッファ仕様＞
　開始位置：テキストバッファトップから何バイト目か（2バイト。下位、上位の順）
　繰り返し数：連続動作の場合の回数（2バイト。下位、上位の順）
　バッファの前からも後ろからも削除できるよう、先頭と最後に命令をおく。ただし、
　後ろの命令コードが10以上の場合は削除された文字そのものを意味する。
1文字挿入
　UNDO_INSERT,開始位置,UNDO_INSERT
1文字上書き
　UNDO_OVERWRITE,開始位置,消去文字 [,消去文字が0?9の場合さらに0を付加]
1文字削除（Delete）
　UNDO_DELETE,開始位置,消去文字 [,消去文字が0?9の場合さらに0を付加]
1文字削除（BackSpace）
　UNDO_BACKSPACE,開始位置,消去文字 [,消去文字が0?9の場合さらに0を付加]
連続挿入（Ctrl+Vで貼り付け）
　UNDO_CONTINS,開始位置,繰り返し数,UNDO_CONTINS
連続削除（領域選択して削除）
　UNDO_CONTDEL,繰り返し数,消去文字列,開始位置,繰り返し数,UNDO_CONTDEL
*/

// アンドゥ用メモリの先頭に1バイトを貯める
// 空きがなくなった場合、最後尾の1命令分を無効化
void pushundomem(unsigned char c){
	unsigned char *p;
	int n;

	if(undobuf_used>=UNDOBUFSIZE){
	//空きがない場合、最後尾のブロックのバイト数分をバッファ利用済み量から減らす
		p=undobuf_top-undobuf_used; //最後尾
		if(p<undobuf) p+=UNDOBUFSIZE;
		switch(*p){
			case UNDO_INSERT: //1文字挿入
				undobuf_used-=4;
				break;
			case UNDO_OVERWRITE: //1文字上書き
			case UNDO_DELETE: //1文字削除
			case UNDO_BACKSPACE: //1文字削除（BS）
				undobuf_used-=4;
				p+=3;
				if(p>=undobuf+UNDOBUFSIZE) p-=UNDOBUFSIZE;
				if(*p<10) undobuf_used--; //コード0?9の場合後ろに0が付加されている
				break;
			case UNDO_CONTINS: //連続挿入
				undobuf_used-=6;
				break;
			case UNDO_CONTDEL: //連続削除
				//繰り返し数の読み出し
				p++;
				if(p>=undobuf+UNDOBUFSIZE) p-=UNDOBUFSIZE;
				n=*p++;
				if(p>=undobuf+UNDOBUFSIZE) p-=UNDOBUFSIZE;
				n+=*p<<8;
				undobuf_used-=n+8;
				break;
		}
	}
	//アンドゥバッファ先頭に1バイト挿入し、先頭位置を1進める
	*undobuf_top++=c;
	if(undobuf_top>=undobuf+UNDOBUFSIZE) undobuf_top-=UNDOBUFSIZE;
	undobuf_used++;
}
// アンドゥバッファに2バイト貯める、下位、上位の順
void pushundomem2(unsigned short w){
	pushundomem((unsigned char)w);
	pushundomem(w>>8);
}
// アンドゥバッファから1バイト読み出し、先頭を1つ戻す
// 戻り値：読み出したコード
unsigned char popundomem(){
	undobuf_top--;
	if(undobuf_top<undobuf) undobuf_top+=UNDOBUFSIZE;
	undobuf_used--;
	return *undobuf_top;
}
// アンドゥバッファから2バイト読み出し
// 戻り値：読み出した2バイトコード
unsigned short popundomem2(){
	unsigned short w;
	w=popundomem()<<8;
	w+=popundomem();
	return w;
}
//アンドゥバッファにデータをセットする
//com:コマンド　1:1文字削除、2:1文字上書き、3:1文字挿入、4:連続削除、5:連続挿入開始
//bp,ix:バッファ上の実行場所（カーソル位置）
//c:文字（上書き、挿入の場合のみ使用）
//n:連続数（連続の場合のみ使用）
void setundobuf(int com,_tbuf *bp,unsigned short ix,unsigned char c,unsigned short n){
	unsigned short pos;

	pos=bpixtopos(bp,ix); //テキストバッファ先頭から何バイト目かを求める
	switch(com){
		case UNDO_INSERT: //1文字挿入
			pushundomem(com);
			pushundomem2(pos);
			pushundomem(com);
			break;
		case UNDO_OVERWRITE: //1文字上書き
		case UNDO_DELETE: //1文字削除（Delete）
		case UNDO_BACKSPACE: //1文字削除（BackSpace）
			pushundomem(com);
			pushundomem2(pos);
			pushundomem(c);
			if(c<10) pushundomem(0); //10未満のコードの場合0を付加
			break;
		case UNDO_CONTINS: //連続挿入
			pushundomem(com);
			pushundomem2(pos);
			pushundomem2(n);
			pushundomem(com);
			break;
		case UNDO_CONTDEL: //連続削除
			pushundomem(com);
			pushundomem2(n);
			break;
	}
}

//テキストバッファbpの先頭からixバイトの位置にcを挿入
//undo 0:通常（アンドゥバッファに格納する）、1:連続挿入中、2:アンドゥ中
//戻り値　成功：0、不正または容量オーバー：-1、空きがあるはずなのに失敗：1
int insertchar(_tbuf *bp,unsigned int ix,unsigned char c,int undo){
	unsigned char *p;

	if(ix > bp->n) return -1; //不正指定
	if(num >= TBUFMAXSIZE) return -1; //バッファ容量オーバー
	if(bp->n < TBUFSIZE){
		//ライン内だけで1バイト挿入可能//
		for(p=bp->Buf + bp->n ; p > bp->Buf+ix ; p--) *p=*(p-1);
		*p=c;
		if(!undo) setundobuf(UNDO_INSERT,bp,ix,0,0); //アンドゥバッファ設定
		bp->n++;
		num++; //バッファ使用量
//		if(bp->n >= TBUFSIZE && bp->next==NULL) newTBuf(bp); //バッファがいっぱいになったら新たにバッファ生成
		return 0;
	}
	//ラインがあふれる場合
	if(bp->next==NULL || bp->next->n >=TBUFSIZE){
		// 最終行または次のラインバッファがいっぱいだったら一行挿入
		if(newTBuf(bp)==NULL){
			// ラインバッファ挿入不可
			return 1;
		}
	}
	if(ix==TBUFSIZE){
		insertchar(bp->next,0,c,undo);
		return 0;
	}
	p=bp->Buf + TBUFSIZE-1;
	insertchar(bp->next,0,*p,1); //次の行の先頭に1文字挿入（必ず空きあり）
	for( ; p > bp->Buf+ix ; p--) *p=*(p-1);
	*p=c;
	if(!undo) setundobuf(UNDO_INSERT,bp,ix,0,0); //アンドゥバッファ設定
	return 0;
}

//テキストバッファbpの先頭からixバイトの位置をcで上書き
//undo 0:通常（アンドゥバッファに格納する）、1:連続中、2:アンドゥ中
//戻り値　成功：0、不正または容量オーバー：-1、空きがあるはずなのに失敗：1
int overwritechar(_tbuf *bp,unsigned int ix,unsigned char c,int undo){

	//現在のバッファ位置の文字が終端または改行の場合、挿入モード
	if(ix > bp->n) return -1; //不正指定
	while(ix >= bp->n){
		if(bp->next==NULL){
			//テキスト全体最後尾の場合は挿入
			return insertchar(bp,ix,c,undo);
		}
		bp=bp->next;
		ix=0;
	}
	if(bp->Buf[ix]=='\n') return insertchar(bp,ix,c,undo);
	if(!undo) setundobuf(UNDO_OVERWRITE,bp,ix,bp->Buf[ix],0); //アンドゥバッファ設定
	bp->Buf[ix]=c;
	return 0;
}

//テキストバッファbpの先頭からixバイトの位置の1バイト削除
//undo -1:通常BackSpace（アンドゥバッファに格納する）
//      0:通常DELETE（アンドゥバッファに格納する）、1:連続中、2:アンドゥ中
void deletechar(_tbuf *bp,unsigned int ix,int undo){
	unsigned char *p;

	if(ix > bp->n) return; //不正指定
	if(ix !=bp->n){
		//バッファの最後の文字より後ろでない場合

		//アンドゥバッファ設定
		if(undo==1) pushundomem(bp->Buf[ix]); //連続削除中
		else if(undo==-1) setundobuf(UNDO_BACKSPACE,bp,ix,bp->Buf[ix],0); //1文字削除(backspace)
		else if(undo==0) setundobuf(UNDO_DELETE,bp,ix,bp->Buf[ix],0); //1文字削除

		for(p=bp->Buf+ix ; p< bp->Buf + bp->n-1 ; p++) *p=*(p+1);
		bp->n--;
		num--; //バッファ使用量
		return;
	}
	//行バッファの現在の最後の場合（削除する文字がない場合）
	if(bp->next==NULL) return; //全体の最後の場合、何もしない
	deletechar(bp->next,0,undo); //次の行の先頭文字を削除
}
//断片化されたテキストバッファの隙間を埋めるガベージコレクション
//カーソルの前と後ろそれぞれ探索して最初の1バイト分のみ実施
//戻り値 1バイトでも移動した場合：1、なかった場合：0
int gabagecollect1(void){

	_tbuf *bp;
	int f=0;
	unsigned char *p,*p2;

	//カーソルがバッファの先頭にある場合、前のバッファの最後尾に変更
	//（ただし前に空きがない場合と先頭バッファの場合を除く）
	while(cursorix==0 && cursorbp->prev!=NULL && cursorbp->prev->n <TBUFSIZE){
		cursorbp=cursorbp->prev;
		cursorix=cursorbp->n;
	}
	//画面左上位置がバッファの先頭にある場合、前のバッファの最後尾に変更
	//（ただし先頭バッファの場合を除く）
	while(disptopix==0 && disptopbp->prev!=NULL){
		disptopbp=disptopbp->prev;
		disptopix=disptopbp->n;
	}
	//カーソルのあるバッファ以外の空バッファを全て削除
	bp=TBufstart;
	while(bp!=NULL){
		if(bp->n == 0 && bp!=cursorbp){
			if(bp==disptopbp) disptopbp=bp->next; //画面左上位置が空バッファ先頭の場合、次にずらす
			bp=deleteTBuf(bp); //空きバッファ削除
		}
		else bp=bp->next;
	}

	//カーソル位置より前の埋まっていないバッファを先頭からサーチ
	bp=TBufstart;
	while(bp->n >= TBUFSIZE){
		if(bp==cursorbp) break;
		bp=bp->next;
	}
	if(bp!=cursorbp){
		//最初に見つけた空き場所に次のバッファから1バイト移動
		bp->Buf[bp->n++] = bp->next->Buf[0];
		bp=bp->next;
		p=bp->Buf;
		p2=p+bp->n-1;
		for( ; p<p2 ; p++) *p=*(p+1);
		bp->n--;
		f=1;
		if(bp == disptopbp) disptopix--;
		if(bp == cursorbp) cursorix--;
//		else if(bp->n == 0) deleteTBuf(bp);
	}
	if(cursorbp->next ==NULL) return f; //カーソル位置が最終バッファなら終了
	//カーソル位置の次のバッファから埋まっていないバッファをサーチ
	bp=cursorbp;
	do{
		bp=bp->next;
		if(bp->next ==NULL) return f; //最終バッファに到達なら終了
	} while(bp->n >=TBUFSIZE);

	//最初に見つけた空き場所に次のバッファから1バイト移動
	bp->Buf[bp->n++] = bp->next->Buf[0];
	bp=bp->next;
	p=bp->Buf;
	p2=p+bp->n-1;
	for( ; p<p2 ; p++) *p=*(p+1);
	bp->n--;
	f=1;
	if(bp->n == 0) deleteTBuf(bp);
	return f;
}
// 変化がなくなるまで1バイト分のガベージコレクションを呼び出し
void gabagecollect2(void){
	while(gabagecollect1()) ;
}
// テキストバッファの初期化
void inittextbuf(void){
	_tbuf *bp;
	for(bp=TextBuffer;bp<TextBuffer+TBUFMAXLINE;bp++) bp->prev=NULL; //未使用バッファ化
	TBufstart=TextBuffer; //リンクの先頭設定
	TBufstart->next=NULL;
	TBufstart->n=0;
	num=0; //バッファ使用量
	edited=0; //編集済みフラグクリア
	undobuf_top=undobuf;
	undobuf_used=0;
}
//画面の再描画
void redraw(){
	unsigned char *vp;
	_tbuf *bp,*bp1,*bp2;
	int ix,ix1,ix2;
	int x,y;
	unsigned char ch,cl;
	unsigned char written;

	vp=TVRAM;
	written=0;
	bp=disptopbp;
	ix=disptopix;
	cl=COLOR_NORMALTEXT;
	if(cursorbp1==NULL){
		//範囲選択モードでない場合
		bp1=NULL;
		bp2=NULL;
	}
	else{
		//範囲選択モードの場合、開始位置と終了の前後判断して
		//bp1,ix1を開始位置、bp2,ix2を終了位置に設定
		if(cy<cy1 || (cy==cy1 && cx<cx1)){
			bp1=cursorbp;
			ix1=cursorix;
			bp2=cursorbp1;
			ix2=cursorix1;
		}
		else{
			bp1=cursorbp1;
			ix1=cursorix1;
			bp2=cursorbp;
			ix2=cursorix;
		}
	}
	for(y=0;y<EDITWIDTHY;y++){
		if(bp==NULL) break;
		for(x=0;x<WIDTH_X;x++){
			//文字がある位置までサーチ
			while(ix>=bp->n){
				if(bp==bp1 && ix==ix1){
					cl=COLOR_AREASELECTTEXT;
				}
				if(bp==bp2 && ix==ix2) cl=COLOR_NORMALTEXT;
				bp=bp->next;
				ix=0;
				if(bp==NULL) break;
			}
			if(bp==NULL) break; //バッファ最終
			if(bp==bp1 && ix==ix1){
				cl=COLOR_AREASELECTTEXT;
			}
			if(bp==bp2 && ix==ix2) cl=COLOR_NORMALTEXT;
			ch=bp->Buf[ix++];
			if(ch=='\n') break;
			if(*vp!=ch || *(vp+ATTROFFSET)!=cl){
				*vp=ch;
				*(vp+ATTROFFSET)=cl;
				written=1;
			}
			vp++;
		}
		//改行およびバッファ最終以降の右側表示消去
		for(;x<WIDTH_X;x++){
			if(*vp!=0 || *(vp+ATTROFFSET)!=0){
				*vp=0;
				*(vp+ATTROFFSET)=0;
				written=1;
			}
			vp++;
		}
	}
	//バッファ最終以降の下側表示消去
	for(;y<EDITWIDTHY;y++){
		for(x=0;x<WIDTH_X;x++){
			if(*vp!=0 || *(vp+ATTROFFSET)!=0){
				*vp=0;
				*(vp+ATTROFFSET)=0;
				written=1;
			}
			vp++;
		}
	}
	if(written) textredraw(); //液晶に出力
}

//カーソルを1つ前に移動
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cy 画面上のカーソル位置
//cx2 cxと同じ
//disptopbp,disptopix 画面左上のバッファ上の位置
void cursor_left(void){
	_tbuf *bp;
	int ix;
	int i;
	int x;

	//バッファ上のカーソル位置を1つ前に移動
	if(cursorix!=0) cursorix--;
	else while(1) {
		//1つ前のバッファの最後尾に移動、ただし空バッファは飛ばす
		if(cursorbp->prev==NULL) return; //テキスト全体先頭なので移動しない
		cursorbp=cursorbp->prev;
		if(cursorbp->n >0){
			cursorix=cursorbp->n-1;//バッファ最後尾
			break;
		}
	}

	//カーソルおよび画面左上位置の更新
	if(cx>0){
		//左端でなければカーソルを単純に1つ左に移動して終了
		cx--;
		cx2=cx;
		return;
	}
	if(cy>0){
		//左端だが上端ではない場合
		if(cursorbp->Buf[cursorix]!='\n'){
			// 移動先が改行コードでない場合、カーソルは1つ上の行の右端に移動
			cx=WIDTH_X-1;
			cx2=cx;
			cy--;
			return;
		}
		//画面左上位置から最後尾のX座標をサーチ
		bp=disptopbp;
		ix=disptopix;
		x=0;
		while(ix!=cursorix || bp!=cursorbp){
			if(bp->n==0){
				//空バッファの場合次へ
				bp=bp->next;
				ix=0;
				continue;
			}
			if(bp->Buf[ix++]=='\n' || x>=WIDTH_X-1) x=0;
			else x++;
			if(ix >= bp->n){
				bp=bp->next;
				ix=0;
			}
		}
		cx=x;
		cx2=cx;
		cy--;
		line_no--;
		return;
	}

	//左端かつ上端の場合
	if(cursorbp->Buf[cursorix]!='\n'){
		// 移動先が改行コードでない場合、カーソルは右端に移動
		// 画面左上位置は画面横幅分前に移動
		cx=WIDTH_X-1;
		cx2=cx;
	}
	else{
		//移動先が改行コードの場合
		//行頭（改行の次の文字またはバッファ先頭）と現在位置の文字数差を
		//画面横幅で割った余りがカーソルX座標
		bp=cursorbp;
		ix=cursorix;
		i=0;
		while(1){
			if(ix==0){
				if(bp->prev==NULL) break;
				bp=bp->prev;
				ix=bp->n;
				continue;
			}
			ix--;
			if(bp->Buf[ix]=='\n') break;
			i++;
		}
		cx=i % WIDTH_X;
		cx2=cx;
		line_no--;
	}
	//画面左上位置は現在位置からX座標分引いたところ
	bp=cursorbp;
	ix=cursorix;
	x=cx;
	while(x>0){
		if(ix==0){
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		x--;
	}
	disptopbp=bp;
	disptopix=ix;
}
//カーソルを1つ後ろに移動
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cy 画面上のカーソル位置
//cx2 cxと同じ
//disptopbp,disptopix 画面左上のバッファ上の位置
void cursor_right(void){

	_tbuf *bp;
	int ix;
	int i;
	int x;
	unsigned char c;

	if(cursorix >= cursorbp->n){
		//バッファ最後尾の場合、次の先頭に移動
		bp=cursorbp;
		while(1) {
			//空バッファは飛ばす
			if(bp->next==NULL) return; //テキスト全体最後尾なので移動しない
			bp=bp->next;
			if(bp->n >0) break;
		}
		cursorbp=bp;
		cursorix=0;//バッファ先頭
	}
	c=cursorbp->Buf[cursorix++]; //バッファ上のカーソル位置のコードを読んで1つ後ろに移動
	if(c!='\n' && cx<WIDTH_X-1){
		//カーソル位置が改行でも右端でもない場合単純に1つ右に移動して終了
		cx++;
		cx2=cx;
		return;
	}
	cx=0; //カーソルを左端に移動
	cx2=cx;
	if(c=='\n') line_no++;
	if(cy<EDITWIDTHY-1){
		//下端でなければカーソルを次行に移動して終了
		cy++;
		return;
	}
	//下端の場合
	//画面左上位置を更新
	//改行コードまたは画面横幅超えるまでサーチ
	bp=disptopbp;
	ix=disptopix;
	x=0;
	while(x<WIDTH_X){
		if(ix >= bp->n){
			bp=bp->next;
			ix=0;
			continue;
		}
		if(bp->Buf[ix++]=='\n') break;
		x++;
	}
	disptopbp=bp;
	disptopix=ix;
}
//カーソルを1つ上に移動
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cy 画面上のカーソル位置
//cx2 移動前のcxと同じ
//disptopbp,disptopix 画面左上のバッファ上の位置
void cursor_up(void){

	_tbuf *bp;
	int ix;
	int i;
	int x;
	unsigned char c;

	//画面幅分前に戻ったところがバッファ上カーソルの移動先
	//途中で改行コードがあれば別の手段で検索
	bp=cursorbp;
	ix=cursorix;
	i=cx2-cx;
	while(i<WIDTH_X){
		if(ix==0){
			if(bp->prev==NULL) return; //バッファ先頭までサーチしたら移動なし
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		if(bp->Buf[ix]=='\n') break;
		i++;
	}
	cursorbp=bp;
	cursorix=ix;
	//画面幅の間に改行コードがなかった場合
	if(i==WIDTH_X){
		cx=cx2;
		//画面上端でなければカーソルを1つ上に移動して終了
		if(cy>0){
			cy--;
			return;
		}
		//画面上端の場合、カーソル位置からX座標分戻ったところが画面左上位置
		x=cx;
		while(x>0){
			if(ix==0){
				bp=bp->prev;
				ix=bp->n;
				continue;
			}
			ix--;
			x--;
		}
		disptopbp=bp;
		disptopix=ix;
		return;
	}
	//改行が見つかった場合
	//行頭（改行の次の文字またはバッファ先頭）と現在位置の文字数差を
	//画面横幅で割った余りを求める
	line_no--;
	i=0;
	while(1){
		if(ix==0){
			if(bp->prev==NULL) break;
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		if(bp->Buf[ix]=='\n') break;
		i++;
	}
	x=i % WIDTH_X; //改行ブロックの最終行の右端
	bp=cursorbp;
	ix=cursorix;
	//バッファ上のカーソル位置は改行ブロックの最終行右端からカーソルX座標分戻る
	//最終行右端のほうが小さい場合、その場所をバッファ上のカーソル位置とする
	while(x>cx2){
		if(ix==0){
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		x--;
	}
	cursorbp=bp;
	cursorix=ix;
	cx=x; //cx2または改行ブロック最終行右端
	if(cy>0){
		//画面上端でなければカーソルを1つ上に移動して終了
		cy--;
		return;
	}
	//画面上端の場合
	//画面左上位置は現在位置からX座標分引いたところ
	while(x>0){
		if(ix==0){
			bp=bp->prev;
			ix=bp->n;
			continue;
		}
		ix--;
		x--;
	}
	disptopbp=bp;
	disptopix=ix;
}
//カーソルを1つ下に移動
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cy 画面上のカーソル位置
//cx2 移動前のcxと同じ
//disptopbp,disptopix 画面左上のバッファ上の位置
void cursor_down(void){

	_tbuf *bp;
	int ix;
	int x;
	unsigned char c;

	//次行の先頭サーチ
	//カーソル位置から画面右端までの間に改行コードがあれば次の文字が先頭
	bp=cursorbp;
	ix=cursorix;
	x=cx;
	while(x<WIDTH_X){
		if(ix>=bp->n){
			if(bp->next==NULL) return; //バッファ最後までサーチしたら移動なし
			bp=bp->next;
			ix=0;
			continue;
		}
		c=bp->Buf[ix];
		ix++;
		x++;
		if(c=='\n'){
			line_no++;
			break;
		}
	}
	//次行先頭からcx2文字数分後ろにサーチ
	x=0;
	while(x<cx2){
		if(ix>=bp->n){
			if(bp->next==NULL) break; //バッファ最後の場合そこに移動
			bp=bp->next;
			ix=0;
			continue;
		}
		if(bp->Buf[ix]=='\n') break; //改行コードの場合そこに移動
		ix++;
		x++;
	}
	cursorbp=bp;
	cursorix=ix;
	cx=x;
	//画面下端でなければカーソルを1つ下に移動して終了
	if(cy<EDITWIDTHY-1){
		cy++;
		return;
	}
	//下端の場合
	//画面左上位置を更新
	//改行コードまたは画面横幅超えるまでサーチ
	bp=disptopbp;
	ix=disptopix;
	x=0;
	while(x<WIDTH_X){
		if(ix >= bp->n){
			bp=bp->next;
			ix=0;
			continue;
		}
		if(bp->Buf[ix++]=='\n') break;
		x++;
	}
	disptopbp=bp;
	disptopix=ix;
}
//カーソルを行先頭に移動
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cx2 0
//cy 変更なし
//disptopbp,disptopix 画面左上のバッファ上の位置（変更なし）
void cursor_home(void){

	//カーソルX座標分前に移動
	while(cx>0){
		if(cursorix==0){
			//空バッファは飛ばす
			cursorbp=cursorbp->prev;
			cursorix=cursorbp->n;
			continue;
		}
		cursorix--;
		cx--;
	}
	cx2=0;
}
//カーソルを行末に移動
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cx2 行末
//cy 変更なし
//disptopbp,disptopix 画面左上のバッファ上の位置（変更なし）
void cursor_end(void){
	//カーソルX座標を画面幅分後ろに移動
	//改行コードまたはバッファ最終があればそこに移動
	while(cx<WIDTH_X-1){
		if(cursorix>=cursorbp->n){
			//空バッファは飛ばす
			if(cursorbp->next==NULL) break;
			cursorbp=cursorbp->next;
			cursorix=0;
			continue;
		}
		if(cursorbp->Buf[cursorix]=='\n') break;
		cursorix++;
		cx++;
	}
	cx2=cx;
}
//PageUpキー
//最上行が最下行になるまでスクロール
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cx2
//cy
//disptopbp,disptopix 画面左上のバッファ上の位置
void cursor_pageup(void){
	_tbuf *bp;
	int ix;
	int i;
	int cy_old;

	cy_old=cy;
	while(cy>0) cursor_up(); // cy==0になるまでカーソルを上に移動
	for(i=0;i<EDITWIDTHY-1;i++){
		//画面行数-1行分カーソルを上に移動
		bp=disptopbp;
		ix=disptopix;
		cursor_up();
		if(bp==disptopbp && ix==disptopix) break; //最上行で移動できなかった場合抜ける
	}
	//元のY座標までカーソルを下に移動、1行も動かなかった場合は最上行に留まる
	if(i>0) while(cy<cy_old) cursor_down();
}
//PageDownキー
//最下行が最上行になるまでスクロール
//出力：下記変数を移動先の値に変更
//cursorbp,cursorix バッファ上のカーソル位置
//cx,cx2
//cy
//disptopbp,disptopix 画面左上のバッファ上の位置
void cursor_pagedown(void){
	_tbuf *bp;
	int ix;
	int i;
	int y;
	int cy_old;

	cy_old=cy;
	while(cy<EDITWIDTHY-1){
		// cy==EDITWIDTH-1になるまでカーソルを下に移動
		y=cy;
		cursor_down();
		if(y==cy) break;// バッファ最下行で移動できなかった場合抜ける
	}
	for(i=0;i<EDITWIDTHY-1;i++){
		//画面行数-1行分カーソルを下に移動
		bp=disptopbp;
		ix=disptopix;
		cursor_down();
		if(bp==disptopbp && ix==disptopix) break; //最下行で移動できなかった場合抜ける
	}
	//下端からさらに移動した行数分、カーソルを上に移動、1行も動かなかった場合は最下行に留まる
	if(i>0) while(cy>cy_old) cursor_up();
}
//カーソルをテキストバッファの先頭に移動
void cursor_top(void){
	cursorbp=TBufstart;
	cursorix=0;
	cursorbp1=NULL; //範囲選択モード解除
	disptopbp=cursorbp;
	disptopix=cursorix;
	cx=0;
	cx2=0;
	cy=0;
	line_no=1;
}

//テキストバッファの指定範囲の文字数をカウント
//範囲は(cursorbp,cursorix)と(cursorbp1,cursorix1)で指定
//後ろ側の一つ前の文字までをカウント
int countarea(void){
	_tbuf *bp1,*bp2;
	int ix1,ix2;
	int n;

	//範囲選択モードの場合、開始位置と終了の前後判断して
	//bp1,ix1を開始位置、bp2,ix2を終了位置に設定
	if(cy<cy1 || (cy==cy1 && cx<cx1)){
		bp1=cursorbp;
		ix1=cursorix;
		bp2=cursorbp1;
		ix2=cursorix1;
	}
	else{
		bp1=cursorbp1;
		ix1=cursorix1;
		bp2=cursorbp;
		ix2=cursorix;
	}
	n=0;
	while(1){
		if(bp1==bp2 && ix1==ix2) return n;
		if(ix1 < bp1->n){
			n++;
			ix1++;
		}
		else{
			bp1=bp1->next;
			ix1=0;
		}
	}
}
//テキストバッファの指定位置から複数文字削除
//bp,ix:削除開始位置
//n:削除する文字数
//undo:0:通常、2:アンドゥ中
void deletearea_len(_tbuf *bp,unsigned int ix,int n,int undo){
	unsigned char *p;
	int i;

	//選択範囲が最初のバッファの最後まである場合
	if(n>=(bp->n - ix)){
		if(!undo){
			p=bp->Buf+ix;
			for(i=ix;i < bp->n;i++) pushundomem(*p++); //アンドゥバッファに格納
		}
		n -= bp->n - ix; //削除文字数減
		num-=bp->n - ix; //バッファ使用量を減数
		bp->n=ix; //ix以降を削除
		bp=bp->next;
		if(bp==NULL) return;
		ix=0;
	}
	//次のバッファ以降、選択範囲の終了位置が含まれないバッファは削除
	while(n>=bp->n){
		if(!undo){
			p=bp->Buf;
			for(i=0;i < bp->n;i++) pushundomem(*p++); //アンドゥバッファに格納
		}
		n-=bp->n; //削除文字数減
		num-=bp->n; //バッファ使用量を減数
		bp=deleteTBuf(bp); //バッファ削除して次のバッファに進む
		if(bp==NULL) return;
	}
	//選択範囲の終了位置を含む場合、1文字ずつ削除
	if(!undo) undo=1;
	while(n>0){
		deletechar(bp,ix,undo); //バッファから1文字削除（numは関数内で1減される）
		n--;
	}
}
//テキストバッファの指定範囲を削除
//範囲は(cursorbp,cursorix)と(cursorbp1,cursorix1)で指定
//後ろ側の一つ前の文字までを削除
//削除後のカーソル位置は選択範囲の先頭にし、範囲選択モード解除する
void deletearea(void){
	_tbuf *bp;
	int ix;
	int n;

	n=countarea(); //選択範囲の文字数カウント
	if(n==0) return;

	//範囲選択の開始位置と終了位置の前後を判断してカーソルを開始位置に設定
	if(cy>cy1 || (cy==cy1 && cx>cx1)){
		cursorbp=cursorbp1;
		cursorix=cursorix1;
		cx=cx1;
		cy=cy1;
		line_no=line_no1;
	}
	cx2=cx;
	cursorbp1=NULL; //範囲選択モード解除

	//bp,ixを開始位置に設定
	bp=cursorbp;
	ix=cursorix;

	setundobuf(UNDO_CONTDEL,bp,ix,0,n); //アンドゥバッファ設定（連続削除開始）
	deletearea_len(bp,ix,n,0); //n文字分削除
	//アンドゥバッファに連続削除終了設定
	pushundomem2(bpixtopos(bp,ix));
	pushundomem2(n);
	pushundomem(UNDO_CONTDEL);
}
// 選択範囲をクリップボードにコピー
void clipcopy(void){
	_tbuf *bp1,*bp2;
	int ix1,ix2;
	char *ps,*pd;

	//範囲選択モードの場合、開始位置と終了の前後判断して
	//bp1,ix1を開始位置、bp2,ix2を終了位置に設定
	if(cy<cy1 || (cy==cy1 && cx<cx1)){
		bp1=cursorbp;
		ix1=cursorix;
		bp2=cursorbp1;
		ix2=cursorix1;
	}
	else{
		bp1=cursorbp1;
		ix1=cursorix1;
		bp2=cursorbp;
		ix2=cursorix;
	}
	ps=bp1->Buf+ix1;
	pd=clipboard;
	clipsize=0;
	while(bp1!=bp2 || ix1!=ix2){
		if(ix1 < bp1->n){
			*pd++=*ps++;
			clipsize++;
			ix1++;
		}
		else{
			bp1=bp1->next;
			ps=bp1->Buf;
			ix1=0;
		}
	}
}
// クリップボードから貼り付け
void clippaste(void){
	int n,i;
	unsigned char *p;

	if(clipsize==0 || num+clipsize>TBUFMAXSIZE) return;
	setundobuf(UNDO_CONTINS,cursorbp,cursorix,0,clipsize); //アンドゥバッファ設定
	p=clipboard;
	for(n=clipsize;n>0;n--){
		i=insertchar(cursorbp,cursorix,*p,1);
		if(i>0){
			//バッファ空きがあるのに挿入失敗の場合
			gabagecollect2(); //全体ガベージコレクション
			i=insertchar(cursorbp,cursorix,*p,1);//テキストバッファに１文字挿入
		}
		if(i!=0) break;//挿入失敗
		cursor_right();//画面上、バッファ上のカーソル位置を1つ後ろに移動
		p++;
	}
}
// カーソルを現在の位置から任意の位置に移動
// pos：移動したいテキストバッファ先頭からのバイト位置
void movecursor(int pos){
	int pos2,d;
	pos2=bpixtopos(cursorbp,cursorix);
	d=pos-pos2;
	if(d==0) return;
	if(d>0){
		while(d>0){
			cursor_right();
			d--;
		}
	}
	else{
		while(d<0){
			cursor_left();
			d++;
		}
	}
}
//アンドゥ実行
void undoexec(){
	unsigned char c,c1;
	_tbuf *bp;
	unsigned short n,ix;
	int pos;

	if(undobuf_used==0) return; //アンドゥバッファ空
	cursorbp1=NULL; //範囲選択解除
	c=popundomem(); //アンドゥバッファ先頭の命令読み出し
	switch(c){
		case UNDO_INSERT: //1文字挿入
			//カーソル移動し1文字削除
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			deletechar(bp,ix,2);
			popundomem(); //dummy read
			break;
		case UNDO_CONTINS: //連続挿入
			//カーソル移動し連続数分削除
			n=popundomem2();
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			deletearea_len(bp,ix,n,2);
			popundomem(); //dummy read
			break;
		case UNDO_CONTDEL: //連続削除
			//カーソル移動し連続数分、コードを読み出して挿入
			n=popundomem2();
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			while(n>0){
				insertchar(bp,ix,popundomem(),2);
				n--;
			}
			popundomem2(); //dummy read
			popundomem(); //dummy read
			break;
		case 0: //0の場合、次の1バイトが実際の有効なコード
			c=popundomem();
		default: //1文字削除（DEL、BS）、1文字上書き
			//カーソル移動し1文字挿入（または上書き）
			pos=popundomem2();
			movecursor(pos);
			bp=postobpix(pos,&ix);
			c1=popundomem();
			if(c1==UNDO_OVERWRITE){
				overwritechar(bp,ix,c,2);
			}
			else{
				insertchar(bp,ix,c,2);
				if(c1==UNDO_BACKSPACE) cursor_right();
			}
			break;
	}
}

//範囲選択モード開始時のカーソル開始位置グローバル変数設定
void set_areamode(){
	cursorbp1=cursorbp;
	cursorix1=cursorix;
	cx1=cx;
	cy1=cy;
	line_no1=line_no;
}
//カーソル関連グローバル変数を一時避難
void save_cursor(void){
	cursorbp_t=cursorbp;
	cursorix_t=cursorix;
	disptopbp_t=disptopbp;
	disptopix_t=disptopix;
	cx_t=cx;
	cy_t=cy;
	line_no_t=line_no;
}
//カーソル関連グローバル変数を一時避難場所から戻す
void restore_cursor(void){
	cursorbp=cursorbp_t;
	cursorix=cursorix_t;
	disptopbp=disptopbp_t;
	disptopix=disptopix_t;
	cx=cx_t;
	cy=cy_t;
	line_no=line_no_t;
}

extern FATFS g_FatFs; // file.c
// SDファイルへの保存や読み込み時にファイルエラーが発生した場合に呼び出す
// 戻り値　0：再実行要求、-1：Escapeで抜けた
int filesystemretry(){
	unsigned short vk;
	setcursorcolor(COLOR_NORMALTEXT);
	printstr((unsigned char *)Message3); //Retry / Quit
	while(1){
		inputchar(); //1文字入力待ち
		vk=vkey & 0xff;
		if(vk==VK_RETURN || vk==VK_SEPARATOR){
			f_mount(&g_FatFs, "", 0);
			return 0;
		}
		if(vk==VK_ESCAPE) return -1;
	}
}
// テキストバッファをテキストファイルに書き込み
// 書き込み成功で0、失敗でエラーコード（負数）を返す
int savetextfile(char *filename){
	FIL Fil;
	FRESULT fr;
	_tbuf *bp;
	int ix,n,i,er;
	unsigned char *ps,*pd;
	er=0;//エラーコード
	i=-1;
	if(f_open(&Fil,filename,FA_CREATE_ALWAYS | FA_WRITE)) return ERR_CANTFILEOPEN;
	bp=TBufstart;
	ix=0;
	ps=bp->Buf;
	do{
		pd=filebuf;
		n=0;
		while(n<FILEBUFSIZE-1){
		//改行コードが2バイトになることを考慮してバッファサイズ-1までとする
			while(ix>=bp->n){
				bp=bp->next;
				if(bp==NULL){
					break;
				}
				ix=0;
				ps=bp->Buf;
			}
			if(bp==NULL) break;
			if(*ps=='\n'){
				*pd++='\r'; //改行コード0A→0D 0Aにする
				n++;
			}
			*pd++=*ps++;
			ix++;
			n++;
		}
		if(n>0){
			f_write(&Fil,filebuf,n,&i);
			if(i!=n) er=ERR_CANTWRITEFILE;
		}
	} while(bp!=NULL && er==0);
	f_close(&Fil);
	return er;
}
// テキストファイルをテキストバッファに読み込み
// 読み込み成功で0、失敗でエラーコード（負数）を返す
int loadtextfile(char *filename){
	FIL Fil;
	FRESULT fr;
	_tbuf *bp;
	int ix,n,i,er;
	unsigned char *ps,*pd;
	er=0;//エラーコード
	if(f_open(&Fil, filename, FA_READ)) return ERR_CANTFILEOPEN;
	inittextbuf();
	bp=TextBuffer;
	ix=0;
	pd=bp->Buf;
	do{
		f_read(&Fil,filebuf,FILEBUFSIZE,&n);
		ps=filebuf;
		for(i=0;i<n;i++){
			if(ix>=TBUFSIZE){
				bp->n=TBUFSIZE;
				bp=newTBuf(bp);
				if(bp==NULL){
					er=ERR_FILETOOBIG;
					break;
				}
				ix=0;
				pd=bp->Buf;
			}
			if(*ps=='\r') ps++; //改行コード0D 0A→0Aにする（単純に0D無視）
			else{
				*pd++=*ps++;
				ix++;
				num++;//バッファ総文字数
				if(num>TBUFMAXSIZE){
					er=ERR_FILETOOBIG;
					break;
				}
			}
		}
	} while(n==FILEBUFSIZE && er==0);
	if(bp!=NULL) bp->n=ix;//最後のバッファの文字数
	f_close(&Fil);
	if(er){
		//エラー発生の場合バッファ、カーソル位置初期化
		inittextbuf();
		cursor_top();
	}
	return er;
}
// ファイルの上書き確認
// ファイルの存在をチェックし、存在する場合キーボードから上書き確認する
// fn:ファイル名へのポインタ
// 戻り値　0：存在しないまたは上書き、-1：上書きしない
int overwritecheck(char *fn){
	unsigned short vk;
	FILINFO fno;
	if(f_stat(fn,&fno)) return 0; //ファイルが存在しない
	setcursorcolor(COLOR_ERRORTEXT);
	printstr(fn);
	printstr(": File Exists\n");
	setcursorcolor(COLOR_NORMALTEXT);
	printstr("Ovewrite:[Enter] / Stop:[ESC]\n");
	while(1){
		inputchar(); //1文字入力待ち
		vk=vkey & 0xff;
		if(vk==VK_RETURN || vk==VK_SEPARATOR) return 0;
		if(vk==VK_ESCAPE) return -1;
	}
}

// ファイルの並べ替え比較関数
static int fnamecmp(const void *s1,const void *s2){
	uint32_t t1,t2;
	t1=(((FILINFO *)s1)->fdate <<16)+((FILINFO *)s1)->ftime;
	t2=(((FILINFO *)s2)->fdate <<16)+((FILINFO *)s2)->ftime;
	switch (filesortby)
	{
	case 0: // A..Z
		return strncmp(((FILINFO *)s1)->fname,((FILINFO *)s2)->fname,12);
	case 1: // NEW..OLD
		return (int)(t2-t1);
	case 2: // Z..A
		return strncmp(((FILINFO *)s2)->fname,((FILINFO *)s1)->fname,12);
	case 3: // OLD..NEW
		return (int)(t1-t2);
	}
	return 0;
}

// タイムスタンプ表示
static void disptimestamp(FILINFO *finfo){
	int k;
	printnum(1980+(finfo->fdate>>9));
	printchar('/');
	k=(finfo->fdate>>5) & 0x0f;
	if(k<10) printchar('0');
	printnum(k);
	printchar('/');
	k=finfo->fdate & 0x1f;
	if(k<10) printchar('0');
	printnum(k);
	printchar(' ');
	k=finfo->ftime>>11;
	if(k<10) printchar('0');
	printnum(k);
	printchar(':');
	k=(finfo->ftime>>5) & 0x3f;
	if(k<10) printchar('0');
	printnum(k);
}

// x,yの位置にファイル名またはディレクトリ名を表示
void printfilename(unsigned char x,unsigned char y,int f,int num_dir){
	if(f==-2){
		setcursor(x,y,COLOR_ERRORTEXT);
		printchar('<');
		printstr("New FILE");
		printchar('>');
	}
	else if(f==-1){
		setcursor(x,y,COLOR_ERRORTEXT);
		printchar('<');
		printstr("New Dir");
		printchar('>');
	}
	else if(f<num_dir){
		setcursor(x,y,COLOR_DIR);
		printchar('[');
		printstr(files[f].fname);
		printchar(']');
		if(show_timestamp){
			setcursor(x+13,y,COLOR_DIR);
			disptimestamp(&files[f]);
		}
	}
	else{
		setcursor(x,y,COLOR_NORMALTEXT);
		printstr(files[f].fname);
		if(show_timestamp){
			setcursor(x+13,y,COLOR_NORMALTEXT);
			disptimestamp(&files[f]);
		}
	}
}

void disp_dir_file_list(int filenum,int num_dir, unsigned char* msg){
	int f;
	int x,y;
	int mx,my;

	if(show_timestamp) mx=1; else mx=WIDTH_X/13;
	my=WIDTH_Y-1;

	//ファイル一覧を画面に表示
	cls();
	setcursor(0,0,COLOR_NORMALTEXT);
	printstr(msg);
	printstr(": ");
	setcursorcolor(4);
	printstr("Select&[Enter] / [ESC]\n");
	for(f=-2;f<filenum;f++){
		x=((f+2)%mx)*13+1;
		y=(f+2)/mx+1;
		if(y>=WIDTH_Y-1) break;
		printfilename(x,y,f,num_dir);
	}
}
// files[]配列に読み込まれたファイルまたはディレクトリを画面表示しキーボードで選択する
// filenum:ファイル＋ディレクトリ数
// num_dir:ディレクトリ数（files[]は先頭からnum_dir-1までがディレクトリ）
// msg:画面上部に表示するメッセージ
// 戻り値
//　files[]の選択されたファイルまたはディレクトリ番号
//　-1：新規ディレクトリ作成、tempfile[]にディレクトリ名
//　-2：新規ファイル作成、tempfile[]にファイル名
//　-3：ESCキーが押された
int select_dir_file(int filenum,int num_dir, unsigned char* msg){
	int top,f,f2;
	int x,y;
	unsigned char vk;
	int mx,my;

	//ファイル一覧を画面に表示
	disp_dir_file_list(filenum,num_dir,msg);

	if(show_timestamp) mx=1; else mx=WIDTH_X/13;
	my=WIDTH_Y-1;
	top=-2;//画面一番先頭のファイル番号
	f=-2;//現在選択中のファイル番号
	while(1){
		setcursor(0,WIDTH_Y-1,COLOR_NORMALTEXT);
		for(x=0;x<WIDTH_X-1;x++) printchar(' '); //最下行のステータス表示を消去
		setcursor(((f+2)%mx)*13,(f-top)/mx+1,5);
		printchar(0x1c);// Right Arrow
		cursor--;
		while(1){
			inputchar();
			vk=vkey & 0xff;
			if(vk) break;
		}
		printchar(' ');
		switch(vk){
			case VK_UP:
			case VK_NUMPAD8:
				//上矢印キー
				if(f-mx>=-2){
					f-=mx;
					if(f<top){
						//画面最上部の場合、下にスクロールして最上部にファイル名表示
						setcursor(WIDTH_X-1,WIDTH_Y-2,COLOR_NORMALTEXT);
						while(cursor>=TVRAM+WIDTH_X*2){
							*cursor=*(cursor-WIDTH_X);
							*(cursor+ATTROFFSET)=*(cursor+ATTROFFSET-WIDTH_X);
							cursor--;
						}
						while(cursor>=TVRAM+WIDTH_X) *cursor--=' ';
						textredraw();
						top-=mx;
						for(int i=0;i<mx;i++){
							printfilename(i*13+1,1,top+i,num_dir);
						}
					}
				}
				break;
			case VK_DOWN:
			case VK_NUMPAD2:
				//下矢印キー
				f2=f-(f+2)%mx;
				if(f2+mx<filenum){
					f+=mx;
					if(f>=filenum) f=filenum-1;
					if(f-top>=(WIDTH_Y-2)*mx){
						//画面最下部の場合、上にスクロールして最下部にファイル名表示
						setcursor(0,1,COLOR_NORMALTEXT);
						while(cursor<TVRAM+WIDTH_X*(WIDTH_Y-2)){
							*cursor=*(cursor+WIDTH_X);
							*(cursor+ATTROFFSET)=*(cursor+ATTROFFSET+WIDTH_X);
							cursor++;
						}
						while(cursor<TVRAM+ATTROFFSET-WIDTH_X) *cursor++=' ';
						textredraw();
						top+=mx;
						f2=f-(f+2)%mx;
						for(int i=0;i<mx;i++){
							if(f2+i>=filenum) break;
							printfilename(i*13+1,WIDTH_Y-2,f2+i,num_dir);
						}
					}
				}
				break;
			case VK_LEFT:
			case VK_NUMPAD4:
				//左矢印キー
				if((f+2)%mx>0) f--;
				break;
			case VK_RIGHT:
			case VK_NUMPAD6:
				//右矢印キー
				if(((f+2)%mx+1)<mx && f+1<filenum) f++;
				break;
			case VK_F1:
				//F1キー タイムスタンプ表示切り替え
				if(WIDTH_X>=30){
					show_timestamp^=1;
					//ファイル一覧を画面に表示
					disp_dir_file_list(filenum,num_dir,msg);
					if(show_timestamp) mx=1; else mx=WIDTH_X/13;
					top=-2;//画面一番先頭のファイル番号
					f=-2;//現在選択中のファイル番号
				}
				break;
			case VK_F2:
				//F2キー 並び順切り替え
				filesortby=(filesortby+1)&3;
				if(files[0].fname[0]=='.' && num_dir>2){
					// 親ディレクトリ(..)は並べ替え対象外
					qsort(&(files[1]),num_dir-1,sizeof(FILINFO),fnamecmp); //ディレクトリ名順に並べ替え
				}
				else if(num_dir>1){
					qsort(files,num_dir,sizeof(FILINFO),fnamecmp); //ディレクトリ名順に並べ替え
				}
				if(filenum-num_dir>1){
					qsort(&(files[num_dir]),filenum-num_dir,sizeof(FILINFO),fnamecmp); //ファイル名順に並べ替え
				}
				//ファイル一覧を画面に表示
				disp_dir_file_list(filenum,num_dir,msg);
				top=-2;//画面一番先頭のファイル番号
				f=-2;//現在選択中のファイル番号
				break;
			case VK_RETURN: //Enterキー
			case VK_SEPARATOR: //テンキーのEnter
				if(f==-2){
					//新規ファイル
					setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
					printstr("Input File Name: ");
					setcursorcolor(COLOR_NORMALTEXT);
					//ファイル名入力
					*tempfile=0;
					if(lineinput(tempfile,8+1+3)<0) break; //ESCキー
					if(*tempfile==0) break; //ファイル名入力なし
				}
				else if(f==-1){
					//新規ディレクトリ
					setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
					printstr("Input Dir Name: ");
					setcursorcolor(COLOR_NORMALTEXT);
					//ディレクトリ名入力
					*tempfile=0;
					if(lineinput(tempfile,8+1+3)<0) break; //ESCキー
					if(f_mkdir(tempfile)){
						setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
						printstr("Cannot Make Directory        ");
						break;
					}
				}
				else{
					//ファイル名またはディレクトリ名をtempfileにコピー
					strcpy(tempfile,files[f].fname);
				}
				return f;
			case VK_ESCAPE:
				//ESCキー
				return -3;
		}
	}
}

// カレントディレクトリでのディレクトリ、.BAS、.TXT、.INIファイル一覧を読み込む
// *p_num_dir:ディレクトリ数を返す
// files[]:ファイル名およびディレクトリ名一覧
// 戻り値　ファイル＋ディレクトリ数
int getfilelist(int *p_num_dir){
	unsigned char *ps,*pd;
	int filenum;
	FRESULT fr;
	DIR dj;		 // Directory object
	FILINFO fno; // File information
	filenum=0;

	//ディレクトリのサーチ
	f_getcwd(temppath,PATHNAMEMAX);
	while(1){
		if(!f_opendir(&dj, temppath)) break;
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Cannot Open Dir\n");
		if(filesystemretry()) return 0; //リトライを促す、あきらめた場合はreturnする
	}
	if (temppath[1]){ // not root directory
		// 親ディレクトリ
		strcpy(files[filenum].fname, "..");
		files[filenum].fdate=20513; // 2020/01/01
		files[filenum].ftime=0;
		files[filenum].fattrib=AM_DIR;
		filenum++;
	}
	while (filenum<MAXFILENUM){
		f_readdir(&dj, &fno); // Read a directory item
		if (fno.fname[0] == 0) break;
		if ((fno.fattrib & AM_DIR) && !(fno.fattrib & AM_SYS)){ // It is a directory
			files[filenum]=fno;
			filenum++;
		}
	}
	f_closedir(&dj);
	*p_num_dir=filenum;
	if(files[0].fname[0]=='.' && filenum>2){
		// 親ディレクトリ(..)は並べ替え対象外
		qsort(&(files[1]),filenum-1,sizeof(FILINFO),fnamecmp); //ディレクトリ名順に並べ替え
	}
	else if(filenum>1){
		qsort(files,filenum,sizeof(FILINFO),fnamecmp); //ディレクトリ名順に並べ替え
	}

	//拡張子 BASファイルのサーチ
	if(filenum<MAXFILENUM){
		fr = f_findfirst(&dj, &fno, temppath, "*.BAS"); // BASICソースファイル
		while (fr == FR_OK && fno.fname[0]){ // Repeat while an item is found
			files[filenum]=fno;
			filenum++;
			if (filenum >= MAXFILENUM) break;
			fr = f_findnext(&dj, &fno); // Search for next item
		}
		f_closedir(&dj);
	}
	//拡張子 TXTファイルのサーチ
	if(filenum<MAXFILENUM){
		fr = f_findfirst(&dj, &fno, temppath, "*.TXT"); // textファイル
		while (fr == FR_OK && fno.fname[0]){ // Repeat while an item is found
			files[filenum]=fno;
			filenum++;
			if (filenum >= MAXFILENUM) break;
			fr = f_findnext(&dj, &fno); // Search for next item
		}
		f_closedir(&dj);
	}
	//拡張子 INIファイルのサーチ
	if(filenum<MAXFILENUM){
		fr = f_findfirst(&dj, &fno, temppath, "*.INI"); // INIファイル
		while (fr == FR_OK && fno.fname[0]){ // Repeat while an item is found
			files[filenum]=fno;
			filenum++;
			if (filenum >= MAXFILENUM) break;
			fr = f_findnext(&dj, &fno); // Search for next item
		}
		f_closedir(&dj);
	}
	//拡張子 HTMファイルのサーチ
	if(filenum<MAXFILENUM){
		fr = f_findfirst(&dj, &fno, temppath, "*.HTM"); // HTMファイル
		while (fr == FR_OK && fno.fname[0]){ // Repeat while an item is found
			files[filenum]=fno;
			filenum++;
			if (filenum >= MAXFILENUM) break;
			fr = f_findnext(&dj, &fno); // Search for next item
		}
		f_closedir(&dj);
	}
	if(filenum-*p_num_dir>1){
		qsort(&(files[*p_num_dir]),filenum-*p_num_dir,sizeof(FILINFO),fnamecmp); //ファイル名順に並べ替え
	}
	return filenum;
}

// カレントディレクトリから相対的にディレクトリ移動
void relative_chdir(unsigned char *path){
	unsigned char *p;
	f_getcwd(temppath,PATHNAMEMAX);
	if (*path == '.'){
		// 親ディレクトリへ移動の場合、temppathから現ディレクトリを削除
		for (p = temppath; *p; p++) ;
		for (p -= 2; *p != '/'; p--) ;
		*(p + 1) = 0;
	}
	else{
		// temppathにディレクトリ名を結合
		for (p = temppath; *p; p++) ;
		if(*(p-1)!='/') *p++='/';
		strcpy(p,path);
	}
	f_chdir(temppath);
}
// 現在のテキストバッファの内容をSDカードに保存
// ow　0:名前を付けて保存　　1:上書き保存
// ファイル名はグローバル変数currentfile[]
// ファイル名はキーボードから変更可能
// 成功した場合currentfileを更新
void save_as(int ow){
	int er;
	int filenum,num_dir,f;

	cls();
	setcursor(0,0,COLOR_NORMALTEXT);
	printstr("Save To SD Card\n");
	if(currentfile[0]==0) ow=0; //ファイル名が設定されていない場合名前を付けて保存

	strcpy(tempfile,currentfile); //currentfileからtempfileにコピー

	//カレントディレクトリを変数cwdpathにコピー
	while(1){
		if(!f_getcwd(cwdpath,PATHNAMEMAX)) break;
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Cannot Get Current Dir\n");
		if(filesystemretry()) return; //リトライを促す、あきらめた場合はreturnする
	}
	//現在のディレクトリのパスを表示
	setcursorcolor(COLOR_NORMALTEXT);
	printstr("Current Directory is\n");
	printstr(cwdpath);
	printchar('\n');
	while(1){
		if(ow==0){
			printstr("Input File Name + [Enter]\n");
			printstr("[ESC] Select File/Dir or Quit\n");
			//ファイル名入力
			if(lineinput(tempfile,8+1+3)<0){
				//ESCキーが押された場合、ファイル選択、ディレクトリ変更画面または終了
				while(1){
					filenum=getfilelist(&num_dir); //ディレクトリ、ファイル名一覧を読み込み
					if(filenum==0) return;
					f=select_dir_file(filenum,num_dir,"Save"); //ファイルの選択
					cls();
					if(f==-3){
						//終了
						f_chdir(cwdpath);//カレントディレクトリを元に戻す
						return;
					}
					else if(f==-2){
						//新規ファイル
						if(overwritecheck(tempfile)==0) break;//上書きチェック
					}
					else if(f<num_dir){
						//新規ディレクトリまたはディレクトリ変更
						relative_chdir(tempfile);//ディレクトリ変更して再度ファイル一覧へ
					}
					else break;
				}
			}
			else{
				if(*tempfile==0) continue; //NULL文字列の場合
				if(overwritecheck(tempfile)) continue;
			}
		}
		printstr("Writing...\n");
		er=savetextfile(tempfile); //ファイル保存、er:エラーコード
		if(er==0){
			printstr("OK");
			f_unlink(TEMPFILENAME); //実行時に生成する一時ファイルを削除
			//tempfileからcurrentfileにコピーして終了
			strcpy(currentfile,tempfile);
			f_getcwd(cwdpath,PATHNAMEMAX); //カレントパスを更新
			edited=0; //編集済みフラグクリア
			sleep_ms(1000);//1秒待ち
			return;
		}
		setcursorcolor(COLOR_ERRORTEXT);
		if(er==ERR_CANTFILEOPEN) printstr("Bad File Name or File Error\n");
		else printstr("Cannot Write\n");
		//リトライを促す、あきらめた場合はreturnする
		if(filesystemretry()){
			f_getcwd(cwdpath,PATHNAMEMAX); //カレントパスを更新
			return;
		}
	}
}

// 新規テキスト作成
void newtext(void){
	unsigned char vk;
	if(edited && num){
		//最終保存後に編集済みの場合、保存の確認
		cls();
		setcursorcolor(COLOR_NORMALTEXT);
		printstr("Save Editing File?\n");
		printstr("Save:[Enter] / Not Save:[ESC]\n");
		while(1){
			inputchar(); //1文字キー入力待ち
			vk=vkey & 0xff;
			if(vk==VK_RETURN || vk==VK_SEPARATOR){
				save_as(0); //名前を付けて保存
				break;
			}
			else if(vk==VK_ESCAPE) break;
		}
	}
	inittextbuf(); //テキストバッファ初期化
	cursor_top(); //カーソルをテキストバッファの先頭に設定
	currentfile[0]=0; //作業中ファイル名クリア
}

// 自動実行プログラムの生成
void msra(void){
}

// SDカードからファイルを選択して読み込み
// currenfile[]にファイル名を記憶
// 対象ファイル拡張子 BASおよびTXT
// 戻り値　0：読み込みを行った　-1：読み込みなし
int fileload(void){
	int filenum,f,er;
	unsigned char vk;
	int num_dir;//ディレクトリ数

	//ファイルの一覧をSDカードから読み出し
	cls();
	if(edited && num){
		//最終保存後に編集済みの場合、保存の確認
		setcursorcolor(COLOR_NORMALTEXT);
		printstr("Save Program Before Load?\n");
		printstr("Save:[Enter] / Not Save:[ESC]\n");
		while(1){
			inputchar(); //1文字キー入力待ち
			vk=vkey & 0xff;
			if(vk==VK_RETURN || vk==VK_SEPARATOR){
				save_as(0); //名前を付けて保存
				break;
			}
			else if(vk==VK_ESCAPE) break;
		}
	}
	//カレントディレクトリを変数cwdpathにコピー
	while(1){
		if(!f_getcwd(cwdpath,PATHNAMEMAX)) break;
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("Cannot Get Current Dir\n");
		if(filesystemretry()) return -1; //リトライを促す、あきらめた場合はreturnする
	}
	while(1){
		filenum=getfilelist(&num_dir); //ディレクトリ、ファイル名一覧を読み込み
		if(filenum==0){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr(".BAS or .TXT File Not Found\n");
			printstr((unsigned char *)Message1);// Hit Any Key
			inputchar(); //1文字入力待ち
			f_chdir(cwdpath);//カレントディレクトリを元に戻す
			return -1;
		}
		//ファイルの選択
		f=select_dir_file(filenum,num_dir,"Load");
		if(f==-3){
			//読み込まずに終了
			f_chdir(cwdpath);//カレントディレクトリを元に戻す
			return -1;
		}
		else if(f==-2){
			//新規ファイルまたはファイル名を入力して読み込み
			er=loadtextfile(tempfile); //テキストバッファにファイル読み込み
			if(er==ERR_CANTFILEOPEN){
				//ファイルが存在しない場合、新規テキスト
				edited=0;
				newtext();
			}
			else if(er==ERR_FILETOOBIG){
				//ファイルサイズエラーの場合、選択画面に戻る
				setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
				printstr("File Too Big                 ");
				sleep_ms(1000); //1秒間ウェイト
				continue;
			}
			strcpy(currentfile,tempfile);//currenfile[]にファイル名をコピー
			f_getcwd(cwdpath,PATHNAMEMAX);//cwdpathをカレントディレクトリのパスに変更
			return 0;
		}
		else if(f<num_dir){
			//新規ディレクトリまたはディレクトリ変更して、再度ファイル一覧画面へ
			relative_chdir(tempfile);
		}
		else{
			er=loadtextfile(files[f].fname); //テキストバッファにファイル読み込み
			if(er==0){
				//cwdpath[]、currenfile[]にパス、ファイル名をコピーして終了
				f_getcwd(cwdpath,PATHNAMEMAX);
				strcpy(currentfile,files[f].fname);
				return 0;
			}
			setcursor(0,WIDTH_Y-1,COLOR_ERRORTEXT);
			if(er==ERR_CANTFILEOPEN) printstr("Cannot Open File             ");
			else if(er=ERR_FILETOOBIG) printstr("File Too Big                 ");
			sleep_ms(1000); //1秒間ウェイト
		}
	}
}
// 画面縦横の切り替え
void changewidth(void){
	set_lcdalign(LCD_ALIGNMENT^HORIZONTAL); //縦横切り替え
	EDITWIDTHY=WIDTH_Y-1; //エディタ画面行数設定
	cursor_top(); //カーソルをテキストバッファの先頭に設定
	redraw(); //再描画
	textredraw(); //液晶に強制出力
}

//KM-BASICコンパイル＆実行
// test 0:コンパイルと実行、0以外:コンパイルのみで終了
void run(int test){
	int er;
	FRESULT fr;
	FIL Fil;
	unsigned int disptoppos,cursorpos;
	int alignment;
	int i,edited1;
	_tbuf *bp;
	unsigned short ix;
	unsigned char *p;

	cls();
	setcursor(0,0,COLOR_NORMALTEXT);
	f_getcwd(cwdpath,PATHNAMEMAX);
	while(1){
		//カレントディレクトリをルートに変更
		if(f_chdir((char *)ROOTDIR)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To Root Dir\n");
			if(filesystemretry()) return; //リトライを促す、あきらめた場合はreturnする
			continue;
		}
		//ルートディレクトリのパス名保存ファイルに実行時パスを保存
		if(f_open(&Fil,WORKDIRFILE,FA_CREATE_ALWAYS | FA_WRITE)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Open Work Dir File\n");
			if(filesystemretry()){
				//リトライを促す、あきらめた場合はカレントディレクトリを戻しreturnする
				f_chdir(cwdpath);
				return;
			}
			continue;
		}
		er=f_puts(cwdpath,&Fil);
		f_close(&Fil);
		if(er<0){
			f_unlink(WORKDIRFILE);
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Write Work Dir File\n");
			if(filesystemretry()){
				//リトライを促す、あきらめた場合はカレントディレクトリを戻しreturnする
				f_chdir(cwdpath);
				return;
			}
			continue;
		}
		break;
	}
	while(1){
		//カレントディレクトリを元に戻す
		if(f_chdir(cwdpath)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To CWD\n");
			if(filesystemretry()) return; //リトライを促す、あきらめた場合はreturnする
			continue;
		}
		//実行用引渡しファイルに保存
		if(savetextfile(TEMPFILENAME)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Write To SD Card\n");
			if(filesystemretry()) return; //リトライを促す、あきらめた場合はreturnする
			continue;
		}
		break;
	}

	//カーソル位置、画面表示位置、画面モードの保存
	disptoppos=bpixtopos(disptopbp,disptopix);
	cursorpos=bpixtopos(cursorbp,cursorix);
	alignment=LCD_ALIGNMENT;
	edited1=edited; //編集済みフラグの一時退避
//	set_lcdalign(HORIZONTAL | (LCD_ALIGNMENT&LCD180TURN)); //デフォルトは横方向で実行

	//KM-BASIC実行
	printstr(INTRODUCE_MACHIKANIA);
	// Compile the code
	int s=time_us_32();
	init_compiler();
	er=compile_file(TEMPFILENAME,0);
	if (!er) er=post_compile();
	printnum(time_us_32()-s);
	printstr(" micro seconds spent for compiling\n");
	printstr("\n");
	if (!er && !test) {
		pre_run();
		run_code();
		post_run();
	}

	setcursorcolor(COLOR_NORMALTEXT);
	bgcolor=0; //バックグランドカラーは黒
	printchar('\n');
	printstr((unsigned char *)Message1);// Hit Any Key
	do usbkb_readkey(); //キーバッファが空になるまで読み出し
	while(vkey!=0);
	inputchar(); //1文字入力待ち
	init_palette();	//カラーパレット初期化
	set_lcdalign(alignment);

	while(1){
		//カレントディレクトリをルートに変更
		if(f_chdir((char *)ROOTDIR)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To Root Dir\n");
			filesystemretry(); //リトライを促す
			continue;
		}
		//ルートディレクトリのパス名保存ファイルからパス名を読み出し
		if(f_open(&Fil,WORKDIRFILE,FA_READ)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Open Work Dir File\n");
			filesystemretry(); //リトライを促す
			continue;
		}
		p=f_gets(cwdpath,PATHNAMEMAX,&Fil);
		f_close(&Fil);
		if(p==NULL){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Read Work Dir File\n");
			filesystemretry(); //リトライを促す
			continue;
		}
		f_unlink(WORKDIRFILE); //パス名保存ファイル削除
		break;
	}
	while(1){
		//カレントディレクトリを元に戻す
		if(f_chdir(cwdpath)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Change To CWD\n");
			filesystemretry(); //リトライを促す
			continue;
		}
		//実行用引渡しファイルから元に戻す
		if(loadtextfile(TEMPFILENAME)){
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Cannot Load From SD Card\n");
			filesystemretry(); //リトライを促す
			continue;
		}
		break;
	}

	if(er && (g_after_classcode || !g_class_id) && (disptopbp=linetobpix(g_error_linenum,&disptopix))){
		//コンパイルエラーの場合
		//カーソルをエラー行で画面トップに移動(classファイルでのエラー除く)
		cursorbp=disptopbp;
		cursorix=disptopix;
		cx=0;
		cx2=0;
		cy=0;
		line_no=g_error_linenum;
		//中央になるようスクロール
		for(i=0;i<EDITWIDTHY/2;i++){
			//画面行数半分カーソルを上に移動
			bp=disptopbp;
			ix=disptopix;
			cursor_up();
			if(bp==disptopbp && ix==disptopix) break; //最上行で移動できなかった場合抜ける
		}
		for(;i>0;i--) cursor_down(); //元のY座標までカーソルを下に移動
	}
	else{
		//カーソルを元の位置に設定
		disptopbp=postobpix(disptoppos,&disptopix);
		cursorbp=postobpix(cursorpos,&cursorix);
	}
	cursorbp1=NULL; //範囲選択モード解除
	clipsize=0; //クリップボードクリア
	edited=edited1;
	f_unlink(TEMPFILENAME);
}
//エディター画面最下行の表示
void displaybottomline(void){
	unsigned char *p;
	unsigned char c;
	int t;
	p=cursor; //カーソル位置の退避
	c=cursorcolor;
	if(shiftkeys() & CHK_SHIFT){
		setcursor(0,WIDTH_Y-1,COLOR_BOTTOMLINE);
		printstr("NEW |    |H/V |TEST|");
		setcursorcolor(COLOR_ERRORTEXT);
		t=TBUFMAXSIZE-num;
		if(t==0) t=1;
		while(t<10000){
			printchar(' ');
			t*=10;
		}
		printstr("LEFT:");
		printnum(TBUFMAXSIZE-num);
	}
	else{
		setcursor(0,WIDTH_Y-1,COLOR_BOTTOMLINE);
		printstr("LOAD|SAVE|    |RUN |");
		setcursorcolor(COLOR_ERRORTEXT);
		t=line_no;
		if(t==0) t=1;
		while(t<10000){
			printchar(' ');
			t*=10;
		}
		printstr("LINE:");
		printnum(line_no);
	}
	cursor=p; //カーソル位置戻し
	cursorcolor=c;
}
// 通常文字入力処理
// k:入力された文字コード
void normal_code_process(unsigned char k){
	int i;

	edited=1; //編集済みフラグ
	if(insertmode || k=='\n' || cursorbp1!=NULL){ //挿入モード
		if(cursorbp1!=NULL) deletearea();//選択範囲を削除
		i=insertchar(cursorbp,cursorix,k,0);//テキストバッファに１文字挿入
		if(i>0){
			//バッファ空きがあるのに挿入失敗の場合
			gabagecollect2(); //全体ガベージコレクション
			i=insertchar(cursorbp,cursorix,k,0);//テキストバッファに１文字挿入
		}
		if(i==0) cursor_right();//画面上、バッファ上のカーソル位置を1つ後ろに移動
	}
	else{ //上書きモード
		i=overwritechar(cursorbp,cursorix,k,0);//テキストバッファに１文字上書き
		if(i>0){
			//バッファ空きがあるのに上書き（挿入）失敗の場合
			//（行末やバッファ最後尾では挿入）
			gabagecollect2(); //全体ガベージコレクション
			i=overwritechar(cursorbp,cursorix,k,0);//テキストバッファに１文字上書き
		}
		if(i==0) cursor_right();//画面上、バッファ上のカーソル位置を1つ後ろに移動
	}
}
// 制御文字入力処理
// k:制御文字の仮想キーコード
// sh:シフト関連キー状態
void control_code_process(unsigned char k,unsigned char sh){
	save_cursor(); //カーソル関連変数退避（カーソル移動できなかった場合戻すため）
	switch(k){
		case VK_LEFT:
		case VK_NUMPAD4:
			 //シフトキー押下していなければ範囲選択モード解除（NumLock＋シフト＋テンキーでも解除）
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD4) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //範囲選択モードでなければ範囲選択モード開始
			if(sh & CHK_CTRL){
				//CTRL＋左矢印でHome
				cursor_home();
				break;
			}
			cursor_left();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//範囲選択モードで画面スクロールがあった場合
				if(cy1<EDITWIDTHY-1) cy1++; //範囲スタート位置もスクロール
				else restore_cursor(); //カーソル位置を戻す（画面範囲外の範囲選択禁止）
			}
			break;
		case VK_RIGHT:
		case VK_NUMPAD6:
			 //シフトキー押下していなければ範囲選択モード解除（NumLock＋シフト＋テンキーでも解除）
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD6) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //範囲選択モードでなければ範囲選択モード開始
			if(sh & CHK_CTRL){
				//CTRL＋右矢印でEnd
				cursor_end();
				break;
			}
			cursor_right();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//範囲選択モードで画面スクロールがあった場合
				if(cy1>0) cy1--; //範囲スタート位置もスクロール
				else restore_cursor(); //カーソル位置を戻す（画面範囲外の範囲選択禁止）
			}
			break;
		case VK_UP:
		case VK_NUMPAD8:
			 //シフトキー押下していなければ範囲選択モード解除（NumLock＋シフト＋テンキーでも解除）
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD8) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //範囲選択モードでなければ範囲選択モード開始
			cursor_up();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//範囲選択モードで画面スクロールがあった場合
				if(cy1<EDITWIDTHY-1) cy1++; //範囲スタート位置もスクロール
				else restore_cursor(); //カーソル位置を戻す（画面範囲外の範囲選択禁止）
			}
			break;
		case VK_DOWN:
		case VK_NUMPAD2:
			 //シフトキー押下していなければ範囲選択モード解除（NumLock＋シフト＋テンキーでも解除）
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD2) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //範囲選択モードでなければ範囲選択モード開始
			cursor_down();
			if(cursorbp1!=NULL && (disptopbp!=disptopbp_t || disptopix!=disptopix_t)){
				//範囲選択モードで画面スクロールがあった場合
				if(cy1>0) cy1--; //範囲スタート位置もスクロール
				else restore_cursor(); //カーソル位置を戻す（画面範囲外の範囲選択禁止）
			}
			break;
		case VK_HOME:
		case VK_NUMPAD7:
			 //シフトキー押下していなければ範囲選択モード解除（NumLock＋シフト＋テンキーでも解除）
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD7) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //範囲選択モードでなければ範囲選択モード開始
			cursor_home();
			break;
		case VK_END:
		case VK_NUMPAD1:
			 //シフトキー押下していなければ範囲選択モード解除（NumLock＋シフト＋テンキーでも解除）
			if((sh & CHK_SHIFT)==0 || (k==VK_NUMPAD1) && (sh & CHK_NUMLK)) cursorbp1=NULL;
			else if(cursorbp1==NULL) set_areamode(); //範囲選択モードでなければ範囲選択モード開始
			cursor_end();
			break;
		case VK_PRIOR: // PageUpキー
		case VK_NUMPAD9:
			 //シフト＋PageUpは無効（NumLock＋シフト＋「9」除く）
			if((sh & CHK_SHIFT) && ((k!=VK_NUMPAD9) || ((sh & CHK_NUMLK)==0))) break;
			cursorbp1=NULL; //範囲選択モード解除
			cursor_pageup();
			break;
		case VK_NEXT: // PageDownキー
		case VK_NUMPAD3:
			 //シフト＋PageDownは無効（NumLock＋シフト＋「3」除く）
			if((sh & CHK_SHIFT) && ((k!=VK_NUMPAD3) || ((sh & CHK_NUMLK)==0))) break;
			cursorbp1=NULL; //範囲選択モード解除
			cursor_pagedown();
			break;
		case VK_DELETE: //Deleteキー
		case VK_DECIMAL: //テンキーの「.」
			edited=1; //編集済みフラグ
			if(cursorbp1!=NULL) deletearea();//選択範囲を削除
			else deletechar(cursorbp,cursorix,0);
			break;
		case VK_BACK: //BackSpaceキー
			edited=1; //編集済みフラグ
			if(cursorbp1!=NULL){
				deletearea();//選択範囲を削除
				break;
			}
			if(cursorix==0 && cursorbp->prev==NULL) break; //バッファ先頭では無視
			cursor_left();
			deletechar(cursorbp,cursorix,-1);
			break;
		case VK_INSERT:
		case VK_NUMPAD0:
			insertmode^=1; //挿入モード、上書きモードを切り替え
			break;
		case 'C':
			//CTRL+C、クリップボードにコピー
			if(cursorbp1!=NULL && (sh & CHK_CTRL)) clipcopy();
			break;
		case 'X':
			//CTRL+X、クリップボードに切り取り
			if(cursorbp1!=NULL && (sh & CHK_CTRL)){
				clipcopy();
				deletearea(); //選択範囲の削除
				edited=1; //編集済みフラグ
			}
			break;
		case 'V':
			//CTRL+V、クリップボードから貼り付け
			if((sh & CHK_CTRL)==0) break;
			if(clipsize==0) break;
			edited=1; //編集済みフラグ
			if(cursorbp1!=NULL){
				//範囲選択している時は削除してから貼り付け
				if(num-countarea()+clipsize<=TBUFMAXSIZE){ //バッファ空き容量チェック
					deletearea();//選択範囲を削除
					clippaste();//クリップボード貼り付け
				}
			}
			else{
				if(num+clipsize<=TBUFMAXSIZE){ //バッファ空き容量チェック
					clippaste();//クリップボード貼り付け
				}
			}
			break;
		case 'S':
			//CTRL+S、SDカードに保存
			if(num==0) break;
			if(sh & CHK_CTRL) save_as(1); //上書き保存
			break;
		case 'O':
			//CTRL+O、ファイル読み込み
			if(sh & CHK_CTRL){
				if(fileload()==0){ //ファイルを選択して読み込み
					//読み込みを行った場合、カーソル位置を先頭に
					cursor_top();
				}
			}
			break;
		case 'N':
			//CTRL+N、新規作成
			if(sh & CHK_CTRL) newtext();
			break;
		case VK_F1: //F1キー
			if(sh & CHK_SHIFT) newtext();//SHIFT+F1キー　新規作成
			else{
				//ファイル読み込み
				if(fileload()==0){ //ファイルを選択して読み込み
					//読み込みを行った場合、カーソル位置を先頭に
					cursor_top();
				}
			}
			break;
		case VK_F2: //F2キー
			if(num==0) break;
			if(sh & CHK_SHIFT) msra(); //create direct running file
			else save_as(0); //ファイル名を付けて保存
			break;
		case VK_F3: //F3キー
			if(sh & CHK_SHIFT) changewidth(); //画面縦横の切り替え
			break;
		case VK_F4: //F4キー
			if(num==0) break;
			if(sh & CHK_SHIFT) run(1); //コンパイルテスト
			else run(0); //コンパイル＆実行
			break;
		case 'Z':
			//CTRL+Z、アンドゥ
			if(sh & CHK_CTRL) undoexec();
			break;
	}
}
//テキストエディター本体
void texteditor(void){
	unsigned char k1,k2,sh;
	FIL Fil;	 /* File object needed for each open file */
	FRESULT fr;

	editormallocp=EDITORRAM;
	TextBuffer=(_tbuf *)editormalloc(sizeof(_tbuf)*TBUFMAXLINE);
	clipboard=editormalloc(ATTROFFSET);
	filebuf=editormalloc(FILEBUFSIZE);
	cwdpath=editormalloc(PATHNAMEMAX);
	temppath=editormalloc(PATHNAMEMAX);
	files=(FILINFO *)editormalloc(sizeof(FILINFO)*MAXFILENUM);
	undobuf=editormalloc(UNDOBUFSIZE);

	inittextbuf(); //テキストバッファ初期化
	currentfile[0]=0; //作業中ファイル名クリア
	cwdpath[0]='/'; //カレントディレクトリをルートに設定
	cwdpath[1]=0;

	//実行時生成一時ファイルがあった場合は読み込む
	if(f_open(&Fil, WORKDIRFILE, FA_READ)==FR_OK){
		f_gets(cwdpath, PATHNAMEMAX, &Fil);
		f_close(&Fil);
		if(f_chdir(cwdpath)==FR_OK && loadtextfile(TEMPFILENAME)==0){
			edited=1;
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("Temporary File Loaded\n");
			printstr("Current Directory is\n");
			setcursorcolor(COLOR_DIR);
			printstr(cwdpath);
			setcursorcolor(COLOR_ERRORTEXT);
			printstr("\nSave To SD Card If Necessary\n");
			setcursorcolor(COLOR_NORMALTEXT);
			printstr((unsigned char *)Message1); //Hit Any Key
			inputchar(); //1文字入力待ち
		}
		else{
			cwdpath[0]='/'; //カレントディレクトリをルートに設定
			cwdpath[1]=0;
			f_chdir(cwdpath); //Change Directory
		}
	}
	cursor_top(); //カーソルをテキストバッファの先頭に移動
	insertmode=1; //0:上書き、1:挿入
	clipsize=0; //クリップボードクリア
	EDITWIDTHY=WIDTH_Y-1;

	while(1){
		redraw();//画面再描画
		setcursor(cx,cy,COLOR_NORMALTEXT);
		getcursorchar(); //カーソル位置の文字を退避（カーソル点滅用）
		blinkcursorchar(); //カーソル点滅させる
		while(1){
			k1=usbkb_readkey(); //キーバッファから読み込み、k1:通常文字入力の場合ASCIIコード
			displaybottomline(); //画面最下行にファンクションキー機能表示
			if(vkey) break;  //キーが押された場合ループから抜ける
			if(cursorbp1==NULL) gabagecollect1(); //1バイトガベージコレクション（範囲選択時はしない）
			blinkcursorchar(); //カーソル点滅させる
			wait60thsec(1);
		}
		resetcursorchar(); //カーソルを元の文字表示に戻す
		k2=(unsigned char)vkey; //k2:仮想キーコード
		sh=vkey>>8;             //sh:シフト関連キー状態
		if(k2==VK_RETURN || k2==VK_SEPARATOR) k1='\n'; //Enter押下は単純に改行文字を入力とする
		if(k1) normal_code_process(k1); //通常文字が入力された場合
		else control_code_process(k2,sh); //制御文字が入力された場合
		if(cursorbp1!=NULL && cx==cx1 && cy==cy1) cursorbp1=NULL;//選択範囲の開始と終了が重なったら範囲選択モード解除
 	}
}
