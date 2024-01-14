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

//#define TBUFMAXLINE 201 //テキストバッファ数
#define TBUFMAXLINE 181 //テキストバッファ数

#define TBUFSIZE 200 //テキストバッファ1つのサイズ
#define TBUFMAXSIZE (TBUFSIZE*(TBUFMAXLINE-1)) //最大バッファ容量（バッファ1行分空ける）
//#define EDITWIDTHX 36 //エディタ画面横幅
//#define EDITWIDTHY 26 //エディタ画面縦幅
#define COLOR_NORMALTEXT 7 //通常テキスト色
#define COLOR_ERRORTEXT 4 //エラーメッセージテキスト色
#define COLOR_AREASELECTTEXT 4 //範囲選択テキスト色
#define COLOR_BOTTOMLINE 5 //画面最下行の色
#define COLOR_DIR 6 //ディレクトリ名表示の色
#define COLOR_INV 128 //モノクロモード時の反転
#define FILEBUFSIZE 256 //ファイルアクセス用バッファサイズ
#define MAXFILENUM 256 //利用可能ファイル最大数
#define PATHNAMEMAX 128 //ワーキングディレクトリパス名の最大値
#define UNDOBUFSIZE 2048 //アンドゥ用バッファサイズ

#define ERR_FILETOOBIG -1
#define ERR_CANTFILEOPEN -2
#define ERR_CANTWRITEFILE -3

#define TEMPFILENAME "~TEMP.BAS" //実行時ソース保存ファイル名
#define WORKDIRFILE "~WORKDIR.TMP" //実行時パス保存ファイル名

#define UNDO_INSERT 1
#define UNDO_OVERWRITE 2
#define UNDO_DELETE 3
#define UNDO_BACKSPACE 4
#define UNDO_CONTINS 5
#define UNDO_CONTDEL 6

#define EDITORRAMSIZE (50*1024)
