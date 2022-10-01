#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/*
	Settings and defaults
*/

#define PC_CONNECT_BUFFER_SIZE 256
char g_serial_port[128]="dev/ttyS0";
int g_baud_rate=B115200;
char g_dir_root[512]="machikap/";

int g_baud_settings[]={
	B0,
	B50,
	B75,
	B110,
	B134,
	B150,
	B200,
	B300,
	B600,
	B1200,
	B1800,
	B2400,
	B4800,
	B9600,
	B19200,
	B38400,
	B57600,
	B115200,
	B230400,
};
char* g_baud_strings[]={
	"0",
	"50",
	"75",
	"110",
	"134",
	"150",
	"200",
	"300",
	"600",
	"1200",
	"1800",
	"2400",
	"4800",
	"9600",
	"19200",
	"38400",
	"57600",
	"115200",
	"230400",
};

/*
	The other global variables
*/

int g_serial_handle;
char g_curdir[512]="/";
int g_file_size;

/*
	Prototypings
*/

void wait4command(const char* command);

void communication_error(void){
	printf("\nAn error occured in PC during the commnucation\n");
	printf("Restart the application and reset MachiKania\n");
	while(1) usleep(1000000);
}

void listfiles(char *path,void* callback){
	DIR *dir;
	struct dirent *dp;
	char path2[256];
	void(*f)(char* path)=callback;
	dir = opendir(path);
	while((dp = readdir(dir)) != NULL){
		strcpy(path2,path);
		strcat(path2,dp->d_name);
		switch(dp->d_type){
			case DT_DIR:
				if ('.'==dp->d_name[0]) break;
				strcat(path2,"/");
				listfiles(path2,callback);
				break;
			case DT_REG:
			default:
				f(path2);
				break;
		}
	}
	closedir(dir);
}

void listfile_callback(char* path){
	printf("%s\n", path);
}

void listusb_callback(char* path){
	int i;
	const char* const avoid_list[]={
		"/dev/fd","/dev/stderr","/dev/stdin","/dev/stdout","/dev/clipboard","/dev/conin","/dev/conout","/dev/cons0",
		"/dev/console","/dev/dsp","/dev/full","/dev/null","/dev/ptmx","/dev/random","/dev/urandom","/dev/windows",
		"/dev/zero","/dev/sd","/dev/sr","/dev/dvd","/dev/snd","/dev/vhost","/dev/cpu","/dev/log","/dev/cdrom",
		"/dev/i2c","/dev/dri","/dev/disk","/dev/block","/dev/char","/dev/vcs","/dev/input","/dev/bus","/dev/loop",
		"/dev/cdrw","/dev/pts","/dev/."
	};
	for(i=0;i<(sizeof avoid_list/sizeof avoid_list[0]);i++){
		if (!strncmp(path,avoid_list[i],strlen(avoid_list[i]))) return;
	}
	if (!strncmp(path,"/dev/ttyACM",11)) printf("\x1b[45m%s\x1b[49m ", path);
	else if (!strncmp(path,"/dev/ttyUSB",11)) printf("\x1b[45m%s\x1b[49m ", path);
	else if (!strncmp(path,"/dev/ttyS",9)) printf("\x1b[45m%s\x1b[49m ", path);
	else printf("%s ", path);
}

void send_cd(char* path){
	int i;
	char c;
	char command[17]="CD:\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08";
	if ('/'==path[0] && 0==path[1]) {
		command[3]='/';
		command[4]=0;
	} else {
		for(i=0;i<13;i++){
			c=path[i];
			if ('a'<=c && c<='z') c-=0x20;
			else if ('/'==c || 0==c) break;
			command[3+i]=c;
		}
		command[3+i]=0;
	}
	write(g_serial_handle,command,16);
	wait4command("OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08");
}

void send_size(int size){
	int i;
	char c;
	char command[17]="SIZE:\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08";
	g_file_size=size;
	snprintf(command+5,10,"%d",size);
	write(g_serial_handle,command,16);
	wait4command("OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08");
}

void send_cp(char* path, char* pcpath){
	FILE* fh;
	int i;
	char c;
	char c2;
	char command[17]="CP:\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08\x08";
	for(i=0;i<13;i++){
		c=path[i];
		if ('a'<=c && c<='z') c-=0x20;
		else if (0==c) break;
		command[3+i]=c;
	}
	command[3+i]=0;
	write(g_serial_handle,command,16);
	wait4command("SENDFILE\x08\x08\x08\x08\x08\x08\x08\x08");
	// Open the file
	fh=fopen(pcpath,"r");
	if (!fh) {
		printf("\nCannot open file: %s",path);
		communication_error();
	}
	// Send the file
	for(i=0;i<g_file_size;i++){
		c=fgetc(fh);
		command[i&15]=c;
		if ((i&15)==15) write(g_serial_handle,command,16);
		else if (i==g_file_size-1) write(g_serial_handle,command,g_file_size&15);
		if ((PC_CONNECT_BUFFER_SIZE-1)==((PC_CONNECT_BUFFER_SIZE-1)&i)) {
			wait4command("OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08");
		}
	}
	// All done
	fclose(fh);
	wait4command("DONEDONE\x08\x08\x08\x08\x08\x08\x08\x08");
}

void copyfile_callback(char* path){
	int dirlen,i,j;
	char* mpath;
	struct stat filestat;
	// Convert to MachiKania path
	mpath=path+strlen(g_dir_root)-1;
	// Determine the length of the directory path
	for(dirlen=strlen(mpath);1<=dirlen;dirlen--){
		if ('/'==mpath[dirlen-1]) break;
	}
	while (strlen(g_curdir)!=dirlen || strncmp(g_curdir,mpath,dirlen)) {
		if (strlen(g_curdir)<dirlen) {
			if (!strncmp(g_curdir,mpath,strlen(g_curdir))) {
				// New directory is in the current directory
				i=j=strlen(g_curdir);
				memcpy(g_curdir,mpath,dirlen);
				while('/'!=g_curdir[j++]);
				g_curdir[j]=0;
				//printf("chdir %s\n", g_curdir+i);
				send_cd(g_curdir+i);	
			} else {
				// Let's go to root
				g_curdir[0]='/';
				g_curdir[1]=0;
				//printf("chdir /\n");
				send_cd("/");
			}
		} else if (strlen(g_curdir)>dirlen) {
			if (!strncmp(g_curdir,mpath,dirlen)) {
				// Current directory is in the new directory
				// Let's go to parent directory
				for(i=strlen(g_curdir)-1;'/'!=g_curdir[i-1];i--);
				g_curdir[i]=0;
				//printf("chdir ..\n");
				send_cd("..");
			} else {
				// Let's go to root
				g_curdir[0]='/';
				g_curdir[1]=0;
				//printf("chdir /\n");
				send_cd("/");
			}
		} else {
			// Let's go to root
			g_curdir[0]='/';
			g_curdir[1]=0;
			//printf("chdir /\n");
			send_cd("/");
		}
		printf("dir:  %s\n", g_curdir);	
	}
	//printf("copy %s\n", mpath+dirlen);
	if (stat(path,&filestat)) communication_error();
	send_size(filestat.st_size);
	send_cp(mpath+dirlen,path);
}

int open_serial(void){
	struct termios tio;
	int fd;
	time_t t=time(NULL);
	while(time(NULL)-t<3){ // Try 3 seconds
		fd = open(g_serial_port, O_RDWR | O_NONBLOCK);
		if (0<=fd) break;
		usleep(1000);
	}
	if (fd<0) return fd;
	// Initialization
	memset(&tio,0,sizeof(tio));
	tio.c_cflag = CS8 | CLOCAL | CREAD;
	tio.c_cc[VTIME] = 100;
	// Baud rate
	cfsetispeed(&tio,g_baud_rate);
	cfsetospeed(&tio,g_baud_rate);
	// Set the device
	tcsetattr(fd,TCSANOW,&tio);
	return fd;
}

void wait4command(const char* command){
	// Note that USB-serial connection doesn't cause error (1 M bytes communication tested)
	int i,j;
	char buf[1024];
	char request[16];
	int fd=g_serial_handle;
	int echo=strcmp(command,"MACHIKAP\x08\x08\x08\x08\x08\x08\x08\x08");
	while(1){
		usleep(1000);
		i=read(fd, buf, sizeof buf);
		if (write(fd,NULL,0)) {
			// The 0 byte write error corresponds to serial connection error
			if (0<=fd) close(fd);
			fd=open_serial();
			if (fd<0) continue;
			g_serial_handle=fd;
		}
		for(j=0;j<i;j++) {
			if (echo) putchar(buf[j]);
			memmove(request,request+1,15);
			request[15]=buf[j];
			if (!strncmp(request,command,16)) return;
			if (!strncmp(request,"NG\x08\x08NG\x08\x08NG\x08\x08NG\x08\x08",16)) communication_error();
		}
 	}
}

void serialtest(void){
	int i,j;
	int fd;
	
	printf("open serial port...");
	fd = open_serial();
	if (fd<0) {
		printf("failed\nTry some from followings (/dev/tty* would be good)\n");
		listfiles("/dev/",listusb_callback);
		return;
	}
	printf("opened as %d\n",fd);
	g_serial_handle=fd;

	while(1){
		// This is the main loop
		printf("Waiting for request...\n");
		wait4command("MACHIKAP\x08\x08\x08\x08\x08\x08\x08\x08");
		printf("Request detected!\n");
		write(fd,"OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08",16);
		printf("Waiting for SENDCMD...");
		wait4command("SENDCMDS\x08\x08\x08\x08\x08\x08\x08\x08");
		printf("OK      \n");
		listfiles(g_dir_root,copyfile_callback);
		write(fd,"DONEDONE\x08\x08\x08\x08\x08\x08\x08\x08",16);
		wait4command("ALL DONE\x08\x08\x08\x08\x08\x08\x08\x08");
		printf("All done!\n");
 	}
}

void open_ini(void){
	int i;
	FILE* fh;
	char buff[128];
	char* line;
	printf("Opening INI file...");
	fh=fopen("./connect.ini","r");
	if (!fh) {
		printf("not found\n");
		return;
	}
	printf("found\n");
	while(line=fgets(buff,128,fh)){
		//printf("%s",line);
		if (!strncmp(line,"SERIALPORT=",11)) {
			for(i=0;i<sizeof g_serial_port;i++){
				if (line[i+11]<=0x20) {
					g_serial_port[i]=0;
					break;
				}
				g_serial_port[i]=line[i+11];
			}
			g_serial_port[sizeof g_serial_port-1]=0;
			printf("Serial port: %s\n",g_serial_port);
		} else if (!strncmp(line,"BAUD=",5)) {
			for(i=(sizeof g_baud_strings/sizeof g_baud_strings[0])-1;0<=i;i--){
				if (strncmp(line+5,g_baud_strings[i],strlen(g_baud_strings[i]))) continue;
				printf("Baud: %s\n",g_baud_strings[i]);
				break;	
			}
		} else if (!strncmp(line,"ROOT=",5)) {
			for(i=0;i<sizeof g_dir_root;i++){
				if (line[i+5]<=0x20) {
					g_dir_root[i]=0;
					break;
				}
				g_dir_root[i]=line[i+5];
			}
			g_dir_root[sizeof g_dir_root-1]=0;
			printf("Transfer files in : %s\n",g_dir_root);
		}
	}
	fclose(fh);
}

int main(void){
	open_ini();
	printf("\nTransfer following files:\n");
	listfiles(g_dir_root,listfile_callback);
	printf("\n");
	serialtest();
	printf("\nPress ctrl+C to quit\n");
	while(1) usleep(1000000);
	return 0;
}

/*
	Connection sequence (P: PC; M: MachiKania):
	
	M: "MACHIKAP\x08\x08\x08\x08\x08\x08\x08\x08"
	P: "OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08"
	M: "SENDCMDS\x08\x08\x08\x08\x08\x08\x08\x08"
	
	P: "CD:DIRNAME\x00\x08\x08\x08\x08\x08"
	M: "OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08" or
	   "NG\x08\x08NG\x08\x08NG\x08\x08NG\x08\x08"
	
	P: "CP:FILENAME.BAS\x00"
	M: "OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08"
	P: "SIZE:1048576\x08\x08\x08"
	M: "SENDFILE\x08\x08\x08\x08\x08\x08\x08\x08"
	P: Send 256 bytes
	M: "OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08"
	P: Send 256 bytes
	M: "OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08"
	...
	P: Send 256 bytes
	M: "OK\x08\x08OK\x08\x08OK\x08\x08OK\x08\x08"
	P: Send last bytes
	M: "DONEDONE\x08\x08\x08\x08\x08\x08\x08\x08"
	
	P: "DONEDONE\x08\x08\x08\x08\x08\x08\x08\x08"
	M: "ALL DONE\x08\x08\x08\x08\x08\x08\x08\x08"
*/