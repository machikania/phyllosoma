/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "./compiler.h"
#include "./api.h"
#include "./core1.h"
#include "./config.h"
#include "./sleep.h"

/*
	clkdiv=138.75, clock=125000000
	440 Hz: 2048 (125000000/138.75/440 = 2047.50) (138.72=125000000/901120)
*/

/*
	c:  1722
	B:  1825
	A#: 1933
	A:  2048
	G#: 2170
	G:  2299
	F#: 2435
	F:  2580
	E:  2734
	D#: 2896
	D:  3067
	C#: 3251
	C:  3444
	Cb: 3650
*/

const static short g_keys[]={
	1933,1722,3251,2896,2580,2435,2170,//  0 7# C# A#m
	1933,1825,3251,2896,2580,2435,2170,//  7 6# F# D#m
	1933,1825,3251,2896,2734,2435,2170,// 14 5# B  G#m
	2048,1825,3251,2896,2734,2435,2170,// 21 4# E  C#m
	2048,1825,3251,3067,2734,2435,2170,// 28 3# A  F#m
	2048,1825,3251,3067,2734,2435,2299,// 35 2# D  Bm
	2048,1825,3444,3067,2734,2435,2299,// 42 1# G  Em
	2048,1825,3444,3067,2734,2580,2299,// 49 0  C  Am
	2048,1933,3444,3067,2734,2580,2299,// 56 1b F  Dm
	2048,1933,3444,3067,2896,2580,2299,// 63 2b Bb Gm
	2170,1933,3444,3067,2896,2580,2299,// 70 3b Eb Cm
	2170,1933,3444,3251,2896,2580,2299,// 77 4b Ab Fm
	2170,1933,3444,3251,2896,2580,2435,// 84 5b Db Bbm
	2170,1933,3650,3251,2896,2580,2435,// 91 6b Gb Ebm
	2170,1933,3650,3251,2896,2734,2435 // 98 7b Cb Abm
};

/*
	2^(1/12)	~= 69433/(2^16)
	1/(2^(1/12) ~= 1933/(2^11)
*/

#define toneFlat(x) ((((unsigned long)(x))*69433)>>16)
#define toneSharp(x) ((((unsigned long)(x))*1933)>>11)

/* local global vars */
static const short* g_tones;
static int g_qvalue;
static int g_lvalue;
static int g_mpoint;
static char* g_mstr;
static int g_mspos;
static unsigned short g_music[32];
static unsigned short g_sound[32];
static unsigned short g_musiclen[32];
static unsigned char g_soundlen[32];
static int g_musicstart;
static int g_musicend;
static int g_musicwait;
static int g_soundstart;
static int g_soundend;
static int g_soundwait;
static int g_soundrepeat;

static char g_sound_mode=0;

static volatile unsigned short g_pwm_wrap;

static FIL g_ofhandle;
static FIL* g_fhandle=0;
static unsigned char* g_wavtable=0;
static int g_wave_readpos;
static int g_wave_writepos;
static int g_pcm_counter;
static char g_wave_enable;

static struct repeating_timer g_wave_timer;

#define WAVE_BUFFER_SIZE 1024

#define SOUND_MODE_NONE 0
#define SOUND_MODE_MUSIC 1
#define SOUND_MODE_WAVE 2

int musicRemaining(int flagsLR){
	// flagsLR is ignored
	return (g_musicend-g_musicstart)&31;
}

void musicint(void){
	// This function is called every 1/60 sec.
	unsigned int i,j;
	switch(g_sound_mode){
		case SOUND_MODE_MUSIC:
			if (g_soundstart!=g_soundend){
				i=g_sound[g_soundstart];
				if (i<0xffff) {
					if (i!=g_pwm_wrap) {
						g_pwm_wrap=i;
						pwm_set_enabled(AUDIO_SLICE, false);
						pwm_set_wrap(AUDIO_SLICE, i-1);
						pwm_set_chan_level(AUDIO_SLICE, AUDIO_CHAN, i/2);
						pwm_set_counter(AUDIO_SLICE, 0);
					}
					pwm_set_enabled(AUDIO_SLICE, true);
				} else {
					pwm_set_enabled(AUDIO_SLICE, false);
				}
				if ((--g_soundwait)<=0) {
					g_soundstart++;
					if (g_soundstart==g_soundend || 31<g_soundstart) {
						g_soundstart=0;
						g_soundrepeat--;
						if (0<g_soundrepeat) {
							g_soundwait=g_soundlen[g_soundstart];
						} else {
							g_soundend=g_soundrepeat=g_soundwait=0;
						}
					} else {
						g_soundwait=g_soundlen[g_soundstart];
					}
				}
				// Shift music data even though without output.
				if (g_musicstart!=g_musicend) {
					if ((--g_musicwait)<=0) {
						g_musicstart++;
						g_musicstart&=31;
						g_musicwait=g_musiclen[g_musicstart];
					}
				}
			} else  if (g_musicstart!=g_musicend) {
				i=g_music[g_musicstart];
				if (i<0xffff) {
					if (i!=g_pwm_wrap) {
						g_pwm_wrap=i;
						pwm_set_enabled(AUDIO_SLICE, false);
						pwm_set_wrap(AUDIO_SLICE, i-1);
						pwm_set_chan_level(AUDIO_SLICE, AUDIO_CHAN, i/2);
						pwm_set_counter(AUDIO_SLICE, 0);
					}
					pwm_set_enabled(AUDIO_SLICE, true);
				} else {
					pwm_set_enabled(AUDIO_SLICE, false);
				}
				if ((--g_musicwait)<=0) {
					g_musicstart++;
					g_musicstart&=31;
					g_musicwait=g_musiclen[g_musicstart];
					if (((g_musicstart+1)&31)==g_musicend) {
						// Raise MUSIC interrupt flag
						raise_interrupt_flag(INTERRUPT_MUSIC);
					}
				}
			} else {
				pwm_set_enabled(AUDIO_SLICE, false);
			}
			break;
		case SOUND_MODE_WAVE:
			if (g_wave_writepos==g_wave_readpos) {
				g_sound_mode=SOUND_MODE_NONE;
				g_wave_enable=0;
				raise_interrupt_flag(INTERRUPT_WAVE);
			}
			break;
		default:
			break;
	}
}

void err_music(char* str){
	printstr(str);
	stop_with_error(ERROR_MUSIC);
}

int musicGetNum(){
	int i, ret;
	char b;
	// Skip non number character
	for(i=0;(b=g_mstr[g_mspos+i])<'0' && '9'<g_mstr[g_mspos+i];i++);
	// Determine the number
	ret=0;
	while('0'<=b && b<='9'){
		ret*=10;
		ret+=b-'0';
		i++;
		b=g_mstr[g_mspos+i];
	}
	g_mspos+=i;
	return ret;
}

void stop_music(void){
	// Initializations for music/sound/wave.
	g_musicstart=g_musicend=g_musicwait=g_soundstart=g_soundend=g_soundwait=g_soundrepeat=0;
	g_sound_mode=SOUND_MODE_NONE;
	g_wave_writepos=g_wave_readpos=0;
	g_pwm_wrap=0;
	
	// Cancel interrupt
	cancel_repeating_timer(&g_wave_timer);

	// Allocate GPIO to the PWM
	gpio_set_function(AUDIO_PORT, GPIO_FUNC_PWM);
	// Set clock divier for fastest frequency
	pwm_set_clkdiv(AUDIO_SLICE, (float)(g_clock_hz/901120));
	// 2048 cycles PWM (an example for 440 Hz)
	pwm_set_wrap(AUDIO_SLICE, 2047);
	// Set duty to 50%
	pwm_set_chan_level(AUDIO_SLICE, AUDIO_CHAN, 1024);
	// Disable
	pwm_set_enabled(AUDIO_SLICE, false);
	// Reset counter
	pwm_set_counter(AUDIO_SLICE, 0);
	// Close WAVE file
	if (g_fhandle) f_close(g_fhandle);
	g_fhandle=0;
	if (g_wavtable) delete_memory(g_wavtable);
	g_wavtable=0;
	g_wave_enable=0;
}

void init_music(void){
	// Initilize music system
	stop_music();
	g_sound_mode=SOUND_MODE_MUSIC;
	
	// Initializations for music/sound.
	g_qvalue=160; // Q: 1/4=90
	g_lvalue=20;   // L: 1/8
	g_tones=&g_keys[49]; // C major
}

void musicSetL(){
	// Set length of a character.
	// Syntax: L:n/m, where n and m are numbers.
	int n,m;
	n=musicGetNum();
	g_mspos++;
	m=musicGetNum();
	g_lvalue=g_qvalue*n/m;
}

void musicSetQ(){
	int i;
	// Syntax: Q:1/4=n, where n is number.
	// Skip "1/4="
	for(i=0;g_mstr[g_mspos+i]!='=';i++);
	g_mspos+=i+1;
	i=musicGetNum();
	if      (i<48)  { g_qvalue=320; /* 1/4=45  */ }
	else if (i<53)  { g_qvalue=288; /* 1/4=50  */ }
	else if (i<60)  { g_qvalue=256; /* 1/4=56  */ }
	else if (i<70)  { g_qvalue=224; /* 1/4=64  */ }
	else if (i<83)  { g_qvalue=192; /* 1/4=75  */ }
	else if (i<102) { g_qvalue=160; /* 1/4=90  */ }
	else if (i<132) { g_qvalue=128; /* 1/4=113 */ }
	else if (i<188) { g_qvalue=96;  /* 1/4=150 */ }
	else            { g_qvalue=64;  /* 1/4=225 */ }
	g_lvalue=g_qvalue>>3;
}

void musicSetK(){
	// Syntax: K:xxx
	if (!strncmp((char*)&(g_mstr[g_mspos]),"A#m",3)) {
			g_mspos+=3;
			g_tones=&g_keys[0];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"D#m",3)) {
			g_mspos+=3;
			g_tones=&g_keys[7];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"G#m",3)) {
			g_mspos+=3;
			g_tones=&g_keys[14];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"C#m",3)) {
			g_mspos+=3;
			g_tones=&g_keys[21];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"F#m",3)) {
			g_mspos+=3;
			g_tones=&g_keys[28];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Bbm",3)) {
			g_mspos+=3;
			g_tones=&g_keys[84];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Ebm",3)) {
			g_mspos+=3;
			g_tones=&g_keys[91];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Abm",3)) {
			g_mspos+=3;
			g_tones=&g_keys[98];
			return;
	}
	if (!strncmp((char*)&(g_mstr[g_mspos]),"C#",2)) {
			g_mspos+=2;
			g_tones=&g_keys[0];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"F#",2)) {
			g_mspos+=2;
			g_tones=&g_keys[7];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Bm",2)) {
			g_mspos+=2;
			g_tones=&g_keys[35];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Em",2)) {
			g_mspos+=2;
			g_tones=&g_keys[42];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Am",2)) {
			g_mspos+=2;
			g_tones=&g_keys[49];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Dm",2)) {
			g_mspos+=2;
			g_tones=&g_keys[56];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Gm",2) || !strncmp((char*)&(g_mstr[g_mspos]),"Bb",2)) {
			g_mspos+=2;
			g_tones=&g_keys[63];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Cm",2) || !strncmp((char*)&(g_mstr[g_mspos]),"Eb",2)) {
			g_mspos+=2;
			g_tones=&g_keys[70];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Fm",2) || !strncmp((char*)&(g_mstr[g_mspos]),"Ab",2)) {
			g_mspos+=2;
			g_tones=&g_keys[77];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Db",2)) {
			g_mspos+=2;
			g_tones=&g_keys[84];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Gb",2)) {
			g_mspos+=2;
			g_tones=&g_keys[91];
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Cb",2)) {
			g_mspos+=2;
			g_tones=&g_keys[98];
			return;
	}
	switch(g_mstr[g_mspos]){
		case 'B':
			g_mspos++;
			g_tones=&g_keys[14];
			return;
		case 'E':
			g_mspos++;
			g_tones=&g_keys[21];
			return;
		case 'A':
			g_mspos++;
			g_tones=&g_keys[28];
			return;
		case 'D':
			g_mspos++;
			g_tones=&g_keys[35];
			return;
		case 'G':
			g_mspos++;
			g_tones=&g_keys[42];
			return;
		case 'C':
			g_mspos++;
			g_tones=&g_keys[49];
			return;
		case 'F':
			g_mspos++;
			g_tones=&g_keys[56];
			return;
		default:
			err_music(g_mstr);
			break;
	}
}
void musicSetM(){
	// Currently do nothing
	musicGetNum();
	musicGetNum();
}

void set_sound(int flagsLR){
	// flagsLR is ignored
	int sound;
	int len;
	int pos;
	int data;
	// Initialize
	g_soundrepeat=g_soundstart=g_soundend=0;
	pos=0;
	do {
		data=lib_read(0,0,0);
		len=data>>16;
		sound=data&0x0000FFFF;
		if (len) {
			g_sound[pos]=sound-1;
			g_soundlen[pos]=len;
			pos++;
			if (32<pos) {
				err_music("Sound data too long.");
				return;
			}
		} else {
			g_soundrepeat=sound;
		}
	} while(len);
	g_soundend=pos;
	g_soundwait=g_soundlen[0];
}

void set_music(char* str, int flagsLR){
	// flagsLR is ignored
	char b;
	unsigned long tone,tonenatural;
	int len;
	g_mstr=str;
	g_mspos=0;
	while(0<(b=g_mstr[g_mspos])){
		if (g_mstr[g_mspos+1]==':') {
			// Set property
			g_mspos+=2;
			switch(b){
				case 'L':
					musicSetL();
					break;
				case 'Q':
					musicSetQ();
					break;
				case 'K':
					musicSetK();
					break;
				case 'M':
					musicSetM();
					break;
				default:
					err_music(str);
					break;
			}	
		} else if ('A'<=b && b<='G' || 'a'<=b && b<='g' || b=='z') {
			g_mspos++;
			if (b=='z') {
				tone=0;
			} else if (b<='G') {
				tone=g_tones[b-'A'];
				tonenatural=g_keys[b-'A'+49];
			} else {
				tone=g_tones[b-'a']>>1;
				tonenatural=g_keys[b-'a'+49]>>1;
			}
			// Check "'"s
			while(g_mstr[g_mspos]=='\''){
				g_mspos++;
				tone>>=1;
			}
			// Check ","s
			while(g_mstr[g_mspos]==','){
				g_mspos++;
				tone<<=1;
				tonenatural<<=1;
			}
			// Check "^","=","_"
			switch(g_mstr[g_mspos]){
				case '^':
					g_mspos++;
					tone=toneSharp(tone);
					break;
				case '_':
					g_mspos++;
					tone=toneFlat(tone);
					break;
				case '=':
					g_mspos++;
					tone=tonenatural;
					break;
				default:
					break;
			}
			// Check number for length
			b=g_mstr[g_mspos];
			if ('0'<=b && b<='9') {
				len=g_lvalue*musicGetNum();
			} else {
				len=g_lvalue;
			}
			if (g_mstr[g_mspos]=='/') {
				g_mspos++;
				len=len/musicGetNum();
			}
			// Update music value array
			// IEC0bits.T4IE=0; // Stop interruption, first.
			if (g_musicstart==g_musicend) {
				g_musicwait=len;
			}
			g_music[g_musicend]=(tone-1)&0x0000FFFF;
			g_musiclen[g_musicend]=len;
			g_musicend++;
			g_musicend&=31;
			//IEC0bits.T4IE=1; // Restart interruption.
		} else {
			err_music(str);
		}
		// Go to next character
		while(0<g_mstr[g_mspos] && g_mstr[g_mspos]<=0x20 || g_mstr[g_mspos]=='|') g_mspos++;
	}
}

int waveRemaining(int mode){
	int i;
	i=g_wave_writepos-g_wave_readpos;
	if (i<0) i+=WAVE_BUFFER_SIZE;
	switch(mode){
		case 1: // current position (header excluded)
			if (!g_fhandle) return 0;
			return f_tell(g_fhandle)-0x2c;
			break;
		case 2: // file size (header excluded)
			if (!g_fhandle) return 0;
			return f_size(g_fhandle)-0x2c;
			break;
		case 0: // remaining
		default:
			if (!g_fhandle) return i;
			return f_size(g_fhandle)-f_tell(g_fhandle)+i;
			break;
	}
}

void wave_core1_readfile(void){
	int i;
	i=g_wave_writepos-g_wave_readpos;
	if (i<0) i+=WAVE_BUFFER_SIZE;
	if (!g_wave_enable) {
		// Force stop
		if (g_fhandle) f_close(g_fhandle);
		g_fhandle=0;
		return;
	}
	if (0==i || 0==g_fhandle) {
		// All done
		return;
	} else if (i<WAVE_BUFFER_SIZE/2) {
		// Load from file
		f_read(g_fhandle,(void*)&g_wavtable[g_wave_writepos],WAVE_BUFFER_SIZE/2,&i);
		i+=g_wave_writepos;
		if (i<WAVE_BUFFER_SIZE) g_wave_writepos=i;
		else g_wave_writepos=i-WAVE_BUFFER_SIZE;
		if (f_eof(g_fhandle)) {
			// End of file
			f_close(g_fhandle);
			g_fhandle=0;
			return;
		}
	}
	// Check at ~100 Hz
	request_core1_callback_at(wave_core1_readfile,time_us_32()+10000);
}

bool repeating_wave_callback(struct repeating_timer *t) {
	int i=g_wave_readpos;
	if (g_wave_readpos==g_wave_writepos) {
		return false;
	}
	pwm_set_chan_level(AUDIO_SLICE, AUDIO_CHAN, g_wavtable[i++]);
	if (i<WAVE_BUFFER_SIZE) g_wave_readpos=i;
	else g_wave_readpos=i-WAVE_BUFFER_SIZE;
	return true;
}

int checkChars(char* str1, char* str2, int num){
	int i;
	for(i=0;i<num;i++){
		if (str1[i]!=str2[i]) return 1;
	}
	return 0;
}

void set_wave(char* filename, int start){
	int i;
	// If previous WAVE is still beeing played, force quit previous one
	if (g_fhandle) f_close(g_fhandle);
	g_fhandle=0;
	g_wave_enable=0;
	g_wave_writepos=g_wave_readpos=0;
	do {
		sleep_ms(1);
	} while (g_sound_mode==SOUND_MODE_WAVE);
	// Exit function if null filename
	if (filename[0]==0x00) return;
	// Alocate 1024 or more bytes buffer if not assigned
	if (!g_wavtable) {
		g_wavtable=alloc_memory(WAVE_BUFFER_SIZE/4,get_permanent_block_number());
	}
	// Open file
	g_fhandle=&g_ofhandle;
	if (f_open(g_fhandle,filename,FA_READ)) {
		g_fhandle=0;
		stop_with_error(ERROR_FILE);
	}
	// Read header and check if monaural 8 bit 16000 Hz.
	if (f_read(g_fhandle,(void*)&g_wavtable[0],0x2c,&g_wave_writepos)) stop_with_error(ERROR_FILE);
	i=0;
	i+=checkChars((char*)&g_wavtable[0],"RIFF",4);                      // Check RIFF
	i+=checkChars((char*)&g_wavtable[8],"WAVEfmt ",8);                  // Check WAVE and fmt
	i+=checkChars((char*)&g_wavtable[16],"\x10\x00\x00\x00\x01\x00",6); // Check if liear PCM
	if (!checkChars((char*)&g_wavtable[22],"\x01\x00\x80\x3e\x00\x00\x80\x3e\x00\x00\x01\x00",12)) {
		// Monaural 16000 Hz
	} else if (!checkChars((char*)&g_wavtable[22],"\x01\x00\x54\x3d\x00\x00\x54\x3d\x00\x00\x01\x00",12)) {
		// Monaural 15700 Hz
	} else {
		i=1;
	}
	i+=checkChars((char*)&g_wavtable[34],"\x08\x00\x64\x61\x74\x61",6); // Check bit # and data
	if (i) err_music("WAVE format error");
	// Support defined start position here to skip file pointer here.
	f_lseek(g_fhandle, start+0x2c);
	// Read first data.
	if (f_read(g_fhandle,(void*)&g_wavtable[0],WAVE_BUFFER_SIZE/2,(unsigned int*)&g_scratch_int[0])) stop_with_error(ERROR_FILE);
	g_wave_writepos=WAVE_BUFFER_SIZE/2;

	// Initialize PWM for PCM
	g_sound_mode=SOUND_MODE_WAVE;
	// Disable
	pwm_set_enabled(AUDIO_SLICE, false);
	// Fastest clock
	pwm_set_clkdiv(AUDIO_SLICE, 1.0);
	// 256 cycles PWM
	pwm_set_wrap(AUDIO_SLICE, 255);
	// Set duty to 50%
	pwm_set_chan_level(AUDIO_SLICE, AUDIO_CHAN, 128);
	// Reset counter
	pwm_set_counter(AUDIO_SLICE, 0);
	// Enable
	pwm_set_enabled(AUDIO_SLICE, true);

	// Start core1 and request callback
	g_wave_enable=1;
	start_core1();
	request_core1_callback(wave_core1_readfile);
	// Fire PCM interrupt
	add_repeating_timer_us(-63, repeating_wave_callback, NULL, &g_wave_timer);
}

int lib_music(int r0, int r1, int r2){
	switch(r2){
		case LIB_MUSIC_MUSIC:
			set_music((char*)r1,r0);
			garbage_collection((char*)r1);
			return r0;
		case LIB_MUSIC_SOUND:
			set_sound(r0);
			return r0;
		case LIB_MUSIC_MUSICFUNC:
			return musicRemaining(3);
		case LIB_MUSIC_PLAYWAVE:
			set_wave((char*)r1,r0);
			garbage_collection((char*)r1);
			return r0;
		case LIB_MUSIC_PLAYWAVEFUNC:
			return waveRemaining(r0);
		default:
			return r0;
	}
}

int music_statement(void){
	g_default_args[2]=3;
	return argn_function(LIB_MUSIC,
		ARG_STRING<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 |
		LIB_MUSIC_MUSIC<<LIBOPTION);
}

int music_function(void){
	return argn_function(LIB_MUSIC,
		ARG_NONE | 
		LIB_MUSIC_MUSICFUNC<<LIBOPTION);
}

int sound_statement(void){
	int e;
	e=restore_statement();
	if (e) return e;
	skip_blank();
	if (','==source[0]) {
		source++;
		e=get_integer();
	} else {
		e=set_value_in_register(0,3);
	}
	g_default_args[1]=3;
	return argn_function(LIB_MUSIC,
		ARG_NONE | 
		LIB_MUSIC_SOUND<<LIBOPTION);
}

int playwave_statement(void){
	g_default_args[2]=0;
	return argn_function(LIB_MUSIC,
		ARG_STRING<<ARG1 | 
		ARG_INTEGER_OPTIONAL<<ARG2 |
		LIB_MUSIC_PLAYWAVE<<LIBOPTION);
}

int playwave_function(void){
	g_default_args[1]=0;
	return argn_function(LIB_MUSIC,
		ARG_INTEGER_OPTIONAL<<ARG1 |
		LIB_MUSIC_PLAYWAVEFUNC<<LIBOPTION);
}
