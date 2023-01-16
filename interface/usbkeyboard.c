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
#include "bsp/board.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "usbkeyboard.h"

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

// If your host terminal support ansi escape code, it can be use to simulate mouse cursor
#define USE_ANSI_ESCAPE   0
#define MAX_REPORT  4

uint16_t volatile usbkb_shiftkey_a; //シフト、コントロールキー等の状態（左右キー区別）
uint8_t volatile usbkb_shiftkey; //シフト、コントロールキー等の状態（左右キー区別なし）
uint16_t keycodebuf[KEYCODEBUFSIZE]; //キーコードバッファ
uint16_t * volatile keycodebufp1; //キーコード書き込み先頭ポインタ
uint16_t * volatile keycodebufp2; //キーコード読み出し先頭ポインタ
semaphore_t keycodebuf_sem; //キーコードバッファ用セマフォ

//公開変数
volatile uint8_t usbkb_keystatus[256]; // 仮想コードに相当するキーの状態（Onの時1）
uint16_t vkey; // usbkb_readkey()関数でセットされるキーコード、上位8ビットはシフト関連キー
uint8_t lockkey=0; // 初期化時にLockキーの状態指定。下位3ビットが<SCRLK><CAPSLK><NUMLK>
uint8_t keytype=0; // キーボードの種類。0：日本語109キー、1：英語104キー

#define USBKBLED_TIMER_INTERVAL 10;
static uint32_t usbkbled_timer=0;
static bool lockkeychanged;
static uint8_t USBKB_dev_addr=0xFF;
static uint8_t USBKB_instance;
static hid_keyboard_report_t usbkb_report;

void lockkeycheck(uint8_t const vk){
	switch (vk)
	{
		case VK_NUMLOCK:
			usbkb_shiftkey_a^=CHK_NUMLK_A;
			usbkb_shiftkey  ^=CHK_NUMLK;
			lockkey ^= KEYBOARD_LED_NUMLOCK;
			lockkeychanged=true;
			break;
		case VK_CAPITAL:
			if((usbkb_shiftkey & CHK_SHIFT)==0) break;
			usbkb_shiftkey_a^=CHK_CAPSLK_A;
			usbkb_shiftkey  ^=CHK_CAPSLK;
			lockkey ^= KEYBOARD_LED_CAPSLOCK;
			lockkeychanged=true;
			break;
		case VK_SCROLL:
			usbkb_shiftkey_a^=CHK_SCRLK_A;
			usbkb_shiftkey  ^=CHK_SCRLK;
			lockkey ^= KEYBOARD_LED_SCROLLLOCK;
			lockkeychanged=true;
			break;
		default:
			break;
	}
}

void shiftkeycheck(uint8_t const modifier){
// SHIFT,ALT,CTRL,Winキーの押下状態を更新
	usbkb_shiftkey_a = (usbkb_shiftkey_a & 0xff00) | modifier;
	usbkb_shiftkey &= CHK_SCRLK | CHK_NUMLK | CHK_CAPSLK;
	if(usbkb_shiftkey_a & (CHK_SHIFT_L | CHK_SHIFT_R)) usbkb_shiftkey|=CHK_SHIFT;
	if(usbkb_shiftkey_a & (CHK_CTRL_L | CHK_CTRL_R)) usbkb_shiftkey|=CHK_CTRL;
	if(usbkb_shiftkey_a & (CHK_ALT_L | CHK_ALT_R)) usbkb_shiftkey|=CHK_ALT;
	if(usbkb_shiftkey_a & (CHK_WIN_L | CHK_WIN_R)) usbkb_shiftkey|=CHK_WIN;
}

// hid keyboard reportをusbkb_reportに取り込む
// 実際の処理は別途定期的に行う
// 複数キー同時押下エラー（キーコード=1が含まれる）場合無視する
void process_kbd_report(hid_keyboard_report_t const *p_new_report) {
	if(p_new_report->keycode[0]!=1) usbkb_report=*p_new_report;
}

// キーコードバッファに書き込み
// バッファフルで書き込めない場合はfalseを返す
static bool pushkeycodebuf(uint16_t k){
	if((keycodebufp2-keycodebufp1==1) || (keycodebufp1-keycodebufp2==KEYCODEBUFSIZE-1)) return false;
	*keycodebufp1++=k;
	if(keycodebufp1==keycodebuf+KEYCODEBUFSIZE) keycodebufp1=keycodebuf;
	return true;
}

// キーコードバッファから読出し
// バッファが空の場合0を返す
static uint16_t popkeycodebuf(void){
	uint16_t vk;
	if(keycodebufp1==keycodebufp2) return 0;
	vk=*keycodebufp2++;
	if(keycodebufp2==keycodebuf+KEYCODEBUFSIZE) keycodebufp2=keycodebuf;
	return vk;
}


// 押下中のHIDキーコードを読み出し、仮想キーコードに変換
// keycodebufにためる
void usbkb_task(void){
	static hid_keyboard_report_t prev_report = {0, 0, {0}}; // previous report to check key released
	static uint16_t oldvkey=0;
	static uint32_t keyrepeattime=0;
	uint16_t vk2;
	hid_keyboard_report_t *p_usbkb_report=&usbkb_report;

	if(!usbkb_mounted()) return;
	vk2=0;
	shiftkeycheck(p_usbkb_report->modifier);
	sem_acquire_blocking(&keycodebuf_sem); //セマフォ許可要求
	for (uint8_t i = 0; i < 6; i++) {
		uint8_t vk;
		if(keytype==1) vk=hidkey2virtualkey_en[p_usbkb_report->keycode[i]];
		else vk=hidkey2virtualkey_jp[p_usbkb_report->keycode[i]];
		if(vk==0) continue;
		vk2=((uint16_t)usbkb_shiftkey<<8)+vk;
		if(usbkb_keystatus[vk]) continue; // 前回も押されていた場合は無視
		if((usbkb_shiftkey & CHK_CTRL)==0) lockkeycheck(vk); //NumLock、CapsLock、ScrollLock反転処理
		if(!pushkeycodebuf(vk2)) break; //キーコードをバッファにためる、バッファがいっぱいの場合無視
	}
	//シフト関連キーが押された場合もキーコードをバッファにためる
	uint8_t modi=p_usbkb_report->modifier & ~prev_report.modifier;
	if(modi & CHK_SHIFT_L) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_LSHIFT);
	if(modi & CHK_SHIFT_R) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_RSHIFT);
	if(modi & CHK_CTRL_L) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_LCONTROL);
	if(modi & CHK_CTRL_R) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_RCONTROL);
	if(modi & CHK_ALT_L) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_LMENU);
	if(modi & CHK_ALT_R) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_RMENU);
	if(modi & CHK_WIN_L) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_LWIN);
	if(modi & CHK_WIN_R) pushkeycodebuf(((uint16_t)usbkb_shiftkey<<8)+VK_RWIN);
	sem_release(&keycodebuf_sem); //セマフォ許可解除
	if(vk2 & 0xff && vk2==oldvkey){
		//キーリピート処理。キーコードバッファが空でない場合はキーリピートしない
		if(time_us_32() >= keyrepeattime && keycodebufp1==keycodebufp2){
			keyrepeattime+=KEYREPEAT2*1000;
			sem_acquire_blocking(&keycodebuf_sem); //セマフォ許可要求
			pushkeycodebuf(vk2);
			sem_release(&keycodebuf_sem); //セマフォ許可解除
		}
	}
	else{
		oldvkey=vk2;
		keyrepeattime=time_us_32()+KEYREPEAT1*1000;
	}
	// 前回押されていたキーステータスをいったん全てクリア
	for (uint8_t i = 0; i < 6; i++) {
		uint8_t vk;
		if(keytype==1) vk=hidkey2virtualkey_en[prev_report.keycode[i]];
		else vk=hidkey2virtualkey_jp[prev_report.keycode[i]];
		if(vk) usbkb_keystatus[vk]=0;
	}
	// 今回押されているキーステータスをセット
	for (uint8_t i = 0; i < 6; i++) {
		uint8_t vk;
		if(keytype==1) vk=hidkey2virtualkey_en[p_usbkb_report->keycode[i]];
		else vk=hidkey2virtualkey_jp[p_usbkb_report->keycode[i]];
		if(vk) usbkb_keystatus[vk]=1;
	}
	// シフト関連キーのステータスを書き換え
	if(p_usbkb_report->modifier & CHK_SHIFT_L) usbkb_keystatus[VK_LSHIFT]=1; else usbkb_keystatus[VK_LSHIFT]=0;
	if(p_usbkb_report->modifier & CHK_SHIFT_R) usbkb_keystatus[VK_RSHIFT]=1; else usbkb_keystatus[VK_RSHIFT]=0;
	if(p_usbkb_report->modifier & CHK_CTRL_L) usbkb_keystatus[VK_LCONTROL]=1; else usbkb_keystatus[VK_LCONTROL]=0;
	if(p_usbkb_report->modifier & CHK_CTRL_R) usbkb_keystatus[VK_RCONTROL]=1; else usbkb_keystatus[VK_RCONTROL]=0;
	if(p_usbkb_report->modifier & CHK_ALT_L) usbkb_keystatus[VK_LMENU]=1; else usbkb_keystatus[VK_LMENU]=0;
	if(p_usbkb_report->modifier & CHK_ALT_R) usbkb_keystatus[VK_RMENU]=1; else usbkb_keystatus[VK_RMENU]=0;
	if(p_usbkb_report->modifier & CHK_WIN_L) usbkb_keystatus[VK_LWIN]=1; else usbkb_keystatus[VK_LWIN]=0;
	if(p_usbkb_report->modifier & CHK_WIN_R) usbkb_keystatus[VK_RWIN]=1; else usbkb_keystatus[VK_RWIN]=0;

	prev_report = *p_usbkb_report;
}

void usbkbled_task(void){
	if(!usbkb_mounted()) return;
	if (board_millis() < usbkbled_timer) return;
	if(lockkeychanged){
		// Set Lock keys LED
		tuh_hid_set_report(USBKB_dev_addr,USBKB_instance,0,HID_REPORT_TYPE_OUTPUT,&lockkey,sizeof(lockkey));
		lockkeychanged=false;
		usbkbled_timer=board_millis()+USBKBLED_TIMER_INTERVAL;
	}
}

//--------------------------------------------------------------------+
// Mouse
//--------------------------------------------------------------------+

void cursor_movement(int8_t x, int8_t y, int8_t wheel)
{
}

static void process_mouse_report(hid_mouse_report_t const * report)
{
}

// Each HID instance can has multiple reports
static struct
{
	uint8_t report_count;
	tuh_hid_report_info_t report_info[MAX_REPORT];
}hid_info[CFG_TUH_HID];

//--------------------------------------------------------------------+
// Generic Report
//--------------------------------------------------------------------+
static void process_generic_report(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
	(void) dev_addr;

	uint8_t const rpt_count = hid_info[instance].report_count;
	tuh_hid_report_info_t* rpt_info_arr = hid_info[instance].report_info;
	tuh_hid_report_info_t* rpt_info = NULL;

	if ( rpt_count == 1 && rpt_info_arr[0].report_id == 0)
	{
		// Simple report without report ID as 1st byte
		rpt_info = &rpt_info_arr[0];
	}else
	{
		// Composite report, 1st byte is report ID, data starts from 2nd byte
		uint8_t const rpt_id = report[0];

		// Find report id in the arrray
		for(uint8_t i=0; i<rpt_count; i++)
		{
			if (rpt_id == rpt_info_arr[i].report_id )
			{
				rpt_info = &rpt_info_arr[i];
				break;
			}
		}

		report++;
		len--;
	}

	if (!rpt_info)
	{
//    printf("Couldn't find the report info for this report !\r\n");
		return;
	}

	// For complete list of Usage Page & Usage checkout src/class/hid/hid.h. For examples:
	// - Keyboard                     : Desktop, Keyboard
	// - Mouse                        : Desktop, Mouse
	// - Gamepad                      : Desktop, Gamepad
	// - Consumer Control (Media Key) : Consumer, Consumer Control
	// - System Control (Power key)   : Desktop, System Control
	// - Generic (vendor)             : 0xFFxx, xx
	if ( rpt_info->usage_page == HID_USAGE_PAGE_DESKTOP )
	{
		switch (rpt_info->usage)
		{
			case HID_USAGE_DESKTOP_KEYBOARD:
				// Assume keyboard follow boot report layout
				process_kbd_report( (hid_keyboard_report_t const*) report );
			break;

			case HID_USAGE_DESKTOP_MOUSE:
				// Assume mouse follow boot report layout
				process_mouse_report( (hid_mouse_report_t const*) report );
			break;

			default: break;
		}
	}
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
	uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

	if(!usbkb_mounted()) return;

	switch (itf_protocol)
	{
		case HID_ITF_PROTOCOL_KEYBOARD:
			process_kbd_report( (hid_keyboard_report_t const*) report );
		break;

		case HID_ITF_PROTOCOL_MOUSE:
			process_mouse_report( (hid_mouse_report_t const*) report );
		break;

		default:
			// Generic report requires matching ReportID and contents with previous parsed report info
			process_generic_report(dev_addr, instance, report, len);
		break;
	}

	// continue to request to receive report
	tuh_hid_receive_report(dev_addr, instance);
}

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
	// Interface protocol (hid_interface_protocol_enum_t)
	uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

	hid_info[instance].report_count = tuh_hid_parse_report_descriptor(hid_info[instance].report_info, MAX_REPORT, desc_report, desc_len);

	if(itf_protocol==1){ //HIDキーボードの場合
		USBKB_dev_addr=dev_addr;
		USBKB_instance=instance;
		usbkb_shiftkey_a=(uint16_t)lockkey<<8; //Lock関連キーを変数lockkeyで初期化
		usbkb_shiftkey=lockkey<<4; //Lock関連キーを変数lockkeyで初期化
		for(int i=0;i<256;i++) usbkb_keystatus[i]=0; //全キー離した状態
		lockkeychanged=true;
		usbkbled_timer=board_millis()+USBKBLED_TIMER_INTERVAL;
	}

	// request to receive report
	// tuh_hid_report_received_cb() will be invoked when report is available
	tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_set_report_complete_cb(uint8_t dev_addr, uint8_t instance, uint8_t report_id, uint8_t report_type, uint16_t len)
{
}
void tuh_hid_report_sent_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
}


// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
	if(dev_addr==USBKB_dev_addr){
		USBKB_dev_addr=0xFF;
	}
}

// USBとキーボード関連初期化
bool usbkb_init(void){
	USBKB_dev_addr=0xff; //USBキーボード未接続
	usbkb_shiftkey_a=(uint16_t)lockkey<<8; //Lock関連キーを変数lockkeyで初期化
	usbkb_shiftkey=lockkey<<4; //Lock関連キーを変数lockkeyで初期化
	keycodebufp1=keycodebuf; //キーコードバッファ初期化
	keycodebufp2=keycodebuf; //キーコードバッファ初期化
	for(int i=0;i<256;i++) usbkb_keystatus[i]=0; //全キー離した状態
	lockkeychanged=false;
	sem_init(&keycodebuf_sem, 1, 1); //キーコードバッファ用セマフォ初期化
	return tusb_init(); //TinyUSB初期化処理
}

// USBインターフェイス監視とキーボードの処理実施
// Core1で呼び出す
void usbkb_polling(void){
	tuh_task();
	usbkb_task();
	usbkbled_task();
}

// USBキーボードが接続されていればtrueを返す
bool usbkb_mounted(void){
	return USBKB_dev_addr!=0xFF?true:false;
}

// 入力された1つのキーのキーコードをグローバル変数vkeyに格納（押されていなければ0を返す）
// 下位8ビット：キーコード
// 上位8ビット：シフト状態
// 英数・記号文字の場合、戻り値としてASCIIコード（それ以外は0を返す）
uint8_t usbkb_readkey(void){
	uint16_t k;
	uint8_t sh;

	sem_acquire_blocking(&keycodebuf_sem); //セマフォ許可要求
	vkey=popkeycodebuf();
	sem_release(&keycodebuf_sem); //セマフォ許可解除
	if(vkey==0) return 0;
	sh=vkey>>8;
	if(sh & (CHK_CTRL | CHK_ALT | CHK_WIN)) return 0;
	k=vkey & 0xff;
	if(keytype==1){
	//英語キーボード
		if(k>='A' && k<='Z'){
			//SHIFTまたはCapsLock（両方ではない）
			if((sh & (CHK_SHIFT | CHK_CAPSLK))==CHK_SHIFT || (sh & (CHK_SHIFT | CHK_CAPSLK))==CHK_CAPSLK)
				return vk2asc2_en[k];
			else return vk2asc1_en[k];
		}
		if(k>=VK_NUMPAD0 && k<=VK_DIVIDE){ //テンキー
			if((sh & (CHK_SHIFT | CHK_NUMLK))==CHK_NUMLK) //NumLock（SHIFT＋NumLockは無効）
				return vk2asc2_en[k];
			else return vk2asc1_en[k];
		}
		if(sh & CHK_SHIFT) return vk2asc2_en[k];
		else return vk2asc1_en[k];
	}

	if(sh & CHK_SCRLK){
	//日本語キーボード（カナモード）
		if(k>=VK_NUMPAD0 && k<=VK_DIVIDE){ //テンキー
			if((sh & (CHK_SHIFT | CHK_NUMLK))==CHK_NUMLK) //NumLock（SHIFT＋NumLockは無効）
				return vk2kana2[k];
			else return vk2kana1[k];
		}
		if(sh & CHK_SHIFT) return vk2kana2[k];
		else return vk2kana1[k];
	}

	//日本語キーボード（英数モード）
	if(k>='A' && k<='Z'){
		//SHIFTまたはCapsLock（両方ではない）
		if((sh & (CHK_SHIFT | CHK_CAPSLK))==CHK_SHIFT || (sh & (CHK_SHIFT | CHK_CAPSLK))==CHK_CAPSLK)
			return vk2asc2_jp[k];
		else return vk2asc1_jp[k];
	}
	if(k>=VK_NUMPAD0 && k<=VK_DIVIDE){ //テンキー
		if((sh & (CHK_SHIFT | CHK_NUMLK))==CHK_NUMLK) //NumLock（SHIFT＋NumLockは無効）
			return vk2asc2_jp[k];
		else return vk2asc1_jp[k];
	}
	if(sh & CHK_SHIFT) return vk2asc2_jp[k];
	else return vk2asc1_jp[k];
}

// SHIFT関連キーの押下状態を返す
// 上位から<0><SCRLK><CAPSLK><NUMLK><Wiin><ALT><SHIFT><CTRL>
uint8_t shiftkeys(void){
	return usbkb_shiftkey;
}
