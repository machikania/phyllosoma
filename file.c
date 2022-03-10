/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"
#include "./api.h"
#include "./debug.h"

/*
	Use following file APIs:
		FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);
		FRESULT f_close (FIL* fp);
		TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);
		#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
*/

FATFS g_FatFs;

FIL* g_FileHandles[2];

void init_file_system(void){
	printstr("Initializing file system, ");
	if (FR_OK==f_mount(&g_FatFs, "", 0)) printstr("done\n");
	else printstr("failed\n");
}

int compile_file(unsigned char* fname, char isclass){
	int e,i,num;
	FIL fpo;
	FIL* fp=&fpo;
	unsigned char* classfile;
	unsigned char* curdir;
	unsigned short* bl;
	// Initialize
	char showfilename=1;
	begin_file_compiler();
	// Store current directory
	if (f_getcwd(g_file_buffer,g_file_buffer_size)) return show_error(ERROR_UNKNOWN,0);
	for(num=0;g_file_buffer[num]!=0x00;num++);
	curdir=cmpdata_insert_string_stack(num+1);
	if (!curdir) return show_error(ERROR_OBJ_TOO_LARGE,0);
	for(i=0;i<=num;i++) curdir[i]=g_file_buffer[i];
	// Open file
	if (f_open(fp,fname,FA_READ)) {
		// Mount and open again
		if (f_mount(&g_FatFs, "", 0)) return show_error(ERROR_FILE,0);
		if (f_open(fp,fname,FA_READ)) {
			if (isclass) {
				// Class file not found in the current directory
				// The class file may be found as "/lib/classname/classname.bas"
				// Change current directory to "/lib/classname"
				for(num=0;fname[num]!='.';num++);
				for(i=0;i<5;i++) g_file_buffer[i]="/lib/"[i];
				for(i=0;i<num;i++) g_file_buffer[i+5]=fname[i];
				g_file_buffer[i+5]=0x00;
				f_chdir(g_file_buffer);
				// Open the class file again
				if (f_open(fp,fname,FA_READ)) {
					// Error
					f_chdir(curdir);
					return show_error(ERROR_NO_CLASS_FILE,0);
				}
				// Succesfully opened the file.
			} else {
				// The main file not found
				return show_error(ERROR_FILE,0);
			}
		}
	}
	// Compile the file until EOF
	while(!f_eof(fp)){
		if (!f_gets(g_file_buffer,g_file_buffer_size,fp)) break;
		e=compile_line(g_file_buffer);
		if (ERROR_COMPILE_CLASS==e) {
			// Compiling a class is needed.
			// Close current file, first
			f_close(fp);
			// Delete all CMPDATA_LINENUM as these will be registered later
			cmpdata_delete_all(CMPDATA_LINENUM);
			// Insert a BL instruction to skip class code
			bl=object;
			object+=2;
			// Compile the class
			classfile=g_class_file;
			e=compile_file(classfile,1);
			if (e) return e;
			e=post_compilling_a_class();
			if (e) return e;
			// BL jump destination is here
			update_bl(bl,object);
			// Delete the string stack stored for file name
			cmpdata_delete_string_stack(classfile);
			// Open current file again, and continue from the beginning
			f_chdir(curdir);
			if (f_open(fp,fname,FA_READ)) return show_error(ERROR_FILE,0);
			begin_file_compiler();
			continue;
		}
		if (showfilename) {
			showfilename=0;
			printstr("Compiling ");
			printstr(fname);
			printchar('\n');
		}
		if (e<0) {
			f_close(fp);
			printstr(g_file_buffer);
			return e;
		}
	}
	f_close(fp);
	// Delete the temporary string for current directory
	cmpdata_delete_string_stack(curdir);
	return end_file_compiler();
}

/*
	File-related statements/functions follow
		FCLOSE [x]
		FGET x,y 関数として呼ばれた場合は、読み込みに成功したバイト数を返す。
		FILE x
		FOPEN x$,y$[,z]
		FPRINT [ xまたはx$またはx# [ ,または; [ yまたはy$またはy# [ ... ]]]]
		FPUT x,y 関数として呼ばれた場合は、読み込みに成功したバイト数を返す。
		FPUTC x
		FREMOVE x$
		FSEEK x
		SETDIR x$
		FEOF()
		FGETC()
		FLEN()
		FSEEK()
		FINPUT$([x])
		GETDIR$()
*/

int lib_file(int r0, int r1, int r2){
	// TODO: here
	switch(r2){
		case FILE_FCLOSE:
			printstr("FILE_FCLOSE");
			//FCLOSE [x]
			//ファイルを閉じる。引数(x)がある場合は、そのファイルハンドルで指定されたファ
			//イルを閉じる。
			break;
			//FRESULT f_close (
			//  FIL* FileObject     // ファイル・オブジェクトへのポインタ 
			//);
		case FILE_FGET:
			printstr("FILE_FGET");
			//FGET x,y
			//バッファー(xに配列として指定)にyバイト読み込む。関数として呼ばれた場合は、
			//読み込みに成功したバイト数を返す。
			break;
			//FRESULT f_read (
			//  FIL* FileObject,    // ファイル・オブジェクト構造体 
			//  void* Buffer,       // 読み出したデータを格納するバッファ 
			//  UINT ByteToRead,    // 読み出すバイト数 
			//  UINT* ByteRead      // 読み出されたバイト数 
			//);
		case FILE_FILE:
			printstr("FILE_FILE");
			//FILE x
			//アクティブなファイルハンドル（１もしくは２）をxに指定する。
			break;
		case FILE_FPUT:
			printstr("FILE_FPUT");
			//FPUT x,y
			//バッファー(xに配列として指定)のyバイト分を書き込む。関数として呼ばれた場合
			//は、書き込みに成功したバイト数を返す。
			break;
			//FRESULT f_write (
			//  FIL* FileObject,     // ファイル・オブジェクト 
			//  const void* Buffer,  // 書き込みデータ 
			//  UINT ByteToWrite,    // 書き込むバイト数 
			//  UINT* ByteWritten    // 書き込まれたバイト数 
			//);
		case FILE_FPUTC:
			printstr("FILE_FPUTC");
			//FPUTC x
			//xで示される１バイトのデーターをファイルに書き込む。関数として呼ばれた場合
			//は、書き込みに成功したバイト数（１もしくは０）を返す。
			break;
			//int f_putc (
			//  TCHAR Chr,        // 書き込む文字 
			//  FIL* FileObject   // ファイル・オブジェクト 
			//);
		case FILE_FREMOVE:
			printstr("FILE_FREMOVE");
			//FREMOVE x$
			//x$で示される名前のファイルを、SDカードから削除する。関数として呼ばれた場合
			//は、削除に成功したか(0)、失敗したか(-1)を返す。
			break;
			//FRESULT f_unlink (
			//  const TCHAR* FileName  // オブジェクト名へのポインタ 
			//);
		case FILE_FSEEK:
			printstr("FILE_FSEEK");
			//FSEEK x
			//xで示されるファイル位置に移動する。
			break;
			//FRESULT f_lseek (
			//  FIL* FileObject,    // ファイル・オブジェクト構造体へのポインタ 
			//  DWORD Offset        // 移動先オフセット 
			//);
		case FILE_SETDIR:
			printstr("FILE_SETDIR");
			//SETDIR x$
			//カレントディレクトリーをx$に移動する。関数として呼ばれた場合、成功すれば0を、
			//エラーが有れば0以外を返す。
			break;
			//FRESULT f_chdir (
			//  const XCHAR* Path // ディレクトリ名へのポインタ 
			//);
		case FILE_FEOF:
			printstr("FILE_FEOF");
			//FEOF()
			//FOPENで開いたファイルの現在のファイル位置が、末端に到達しているかどうか
			//を返す。１で末端に到達、０で未到達。
			break;
			//int f_eof (
			//  FIL* FileObject   // ファイル・オブジェクト 
			//);
		case FILE_FGETC:
			printstr("FILE_FGETC");
			//FGETC()
			//FOPENで開いたファイルから１バイト読み込み、整数値として返す。ファイル
			//末端に到達しているなどで読み込みに失敗した場合、-1を返す。
			break;
			//TCHAR* f_gets (
			//  TCHAR* Str,       // バッファ 
			//  int Size,         // バッファのサイズ 
			//  FIL* FileObject   // ファイル・オブジェクト 
			//);
		case FILE_FLEN:
			printstr("FILE_FLEN");
			//FLEN()
			//FOPENで開いたファイルのファイル長を、バイト数で返す。
			break;
			//DWORD f_size (
			//  FIL* FileObject   // ファイル・オブジェクト 
			//);
		case FILE_FSEEKFUNC:
			printstr("FILE_FSEEKFUNC");
			//FSEEK()
			//FOPENで開いたファイルの、現在のファイル位置を返す。
			break;
			//DWORD f_tell (
			//  FIL* FileObject   // ファイル・オブジェクト 
			//);
		case FILE_FINPUT:
			printstr("FILE_FINPUT");
			//FINPUT$([x])
			//FOPENで開いたファイルから、xで示された長さの文字列を読み込む。xが省略された
			//場合は、行の最後まで読み込む(改行コードが含まれる)。
			break;
			//TCHAR* f_gets (
			//  TCHAR* Str,       // バッファ 
			//  int Size,         // バッファのサイズ 
			//  FIL* FileObject   // ファイル・オブジェクト 
			//);
		case FILE_GETDIR:
			printstr("FILE_GETDIR");
			//GETDIR$()
			//カレントディレクトリーを文字列として返す。
			break;
			//FRESULT f_getcwd (
			//  TCHAR* Buffer,  // バッファ 
			//  UINT BufferLen  // バッファ・サイズ 
			//);
		default:
			break;
	}
	printstr(" r0: ");
	printint(r0);
	printstr(" r1: ");
	printint(r1);
	printchar('\n');
	return 0;
}

int lib_fopen(int r0, int r1, int r2){
	//FOPEN x$,y$[,z]
	//x$で示される名前のファイルを、y$で示されたモードで開く。同時に開けるファイ
	//ルの数は、２つまで。関数として呼ばれた場合は、ファイルハンドルを返す。y$と
	//しては、次のものが有効。
	//	"r"  ：ファイルを読み込みモードで開く。
	//	"r+" ："r"と同じだが書き込みも可能。
	//	"w"  ：ファイルを書き込みモードで開く。同名のファイルが在る場合は、
	//	       以前のファイルは消去される。
	//	"w+" ："w"と同じだが、読み込みも可能。
	//	"a"  ：ファイルを書き込みモードで開く。同名のファイルが在る場合は、
	//	       ファイルは消去されず、ファイルの最後尾から書き込まれる。
	//	"a+" ："a"と同じだが、読み込みも可能。
	//zには、割り当てたいファイルハンドル（１もしくは２）を指定する。省略した場
	//合、１が指定される。
	
	// TODO: here
	printstr("FILE_FOPEN r0: ");
	printint(r0);
	printstr(" r1: ");
	printint(r1);
	printstr(" r2: ");
	printint(r2);
	printchar('\n');
	return 0;
	//FRESULT f_open (
	//  FIL* FileObject,       // 空のファイル・オブジェクト構造体へのポインタ 
	//  const TCHAR* FileName, // ファイルのフルパス名へのポインタ 
	//  BYTE ModeFlags         // モードフラグ 
	//);

}
int lib_fprint(int r0, int r1, int r2){
	// TODO: here
	printstr(" FPRINT r0: ");
	printint(r0);
	printstr(" r1: ");
	printint(r1);
	printstr(" r2: ");
	printint(r2);
	printchar('\n');
	return 0;
}

int fclose_statement(void){
	g_default_args[1]=0;
	return argn_function(LIB_FILE,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		FILE_FCLOSE<<LIBOPTION);
}
int fget_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER<<ARG2 |
		FILE_FGET<<LIBOPTION);
}
int file_statement(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FILE<<LIBOPTION);
}
int fopen_function(void){
	g_default_args[3]=1;
	return argn_function(LIB_FOPEN,
		ARG_STRING<<ARG1 | 
		ARG_STRING<<ARG2 | 
		ARG_INTEGER_OPTIONAL<<ARG3);
}
int fprint_statement(void){
	//FPRINT [ xまたはx$またはx# [ ,または; [ yまたはy$またはy# [ ... ]]]]
	//PRINT命令と同じだが、画面ではなくファイルに情報が書き込まれる。
	// TODO: here
}
int fput_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		ARG_INTEGER<<ARG2 |
		FILE_FPUT<<LIBOPTION);
}
int fputc_function(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FPUTC<<LIBOPTION);
}
int fremove_function(void){
	return argn_function(LIB_FILE,
		ARG_STRING<<ARG1 |
		FILE_FREMOVE<<LIBOPTION);
}
int fseek_statement(void){
	return argn_function(LIB_FILE,
		ARG_INTEGER<<ARG1 |
		FILE_FSEEK<<LIBOPTION);
}
int setdir_function(void){
	return argn_function(LIB_FILE,
		ARG_STRING<<ARG1 |
		FILE_SETDIR<<LIBOPTION);
}
int feof_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FEOF<<LIBOPTION);
}
int fgetc_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FGETC<<LIBOPTION);
}
int flen_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FLEN<<LIBOPTION);
}
int fseek_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_FSEEKFUNC<<LIBOPTION);
}
int finput_function(void){
	g_default_args[1]=-1;
	return argn_function(LIB_FILE,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		FILE_FINPUT<<LIBOPTION);
}
int getdir_function(void){
	return argn_function(LIB_FILE,
		ARG_NONE | 
		FILE_GETDIR<<LIBOPTION);
}
