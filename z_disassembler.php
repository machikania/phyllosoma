<?php

date_default_timezone_set('America/Los_Angeles');

/*
KM-BASIC for ARM
Compiling CLASS1.BAS
Compiling main.bas
6387 micro seconds spent for compiling

kmbasic_object:20002E48
F000 F812 2383 47C0 0003 0000 001A 0032 001B 0012 001C 0002 2E50 2000 0000 0000 0000 0000 0000 0000 4800 E001 0102 0000 231B 47C0 63A8 2300 68BA 8393 6830 B083 9601 466E 6030 2000 60B0 F000 F803 B500 F000 F819 F7FF FFFB 0031 2315 47C0 6876 B003 6BA8 211B 231C 47C0 B401 207B BC02 6008 6BA8 211B 231C 47C0 6800 2100 2381 47C0 2383 47C0 2000 6228 2300 68BA 8213 2003 B401 6A28 F000 F808 2003 B401 6A28 3001 6228 2300 68BA 8213 BC02 4281 DA01 F000 F810 6BA8 B401 6A28 0080 BC02 5808 B401 2000 BC02 2302 47C0 2121 2381 47C0 F7FF FFE3 BD00 0102 0000 0000 2E50 2000 2E60 2000

END: 2383 47C0
call_lib_code(LIB_OBJ_FIELD): 231C 47C0

Class ID list
0102
Class list
20002E50
Class structures
20002E50: 0032001A  0012001B
20002E50        0               0               0
Not public field/method in line 4
-
*/
$code="
kmbasic_object:20002E48
F000 F80E 2383 47C0 0002 0000 001B 0012 001C 0002 2E50 2000 0000 0000 0000 0000 F000 F80E 2383 47C0 0002 0000 001E 0002 001F 0012 2E70 2000 0000 0000 0000 0000 4800 E001 0102 0000 231B 47C0 63A8 2300 68BA 8393 4800 E001 0104 0000 231B 47C0 63E8 2300 68BA 83D3 6830 B083 9601 466E 6030 2000 60B0 F000 F803 B500 F000 F83F F7FF FFFB 0031 2315 47C0 6876 B003 6BA8 211B 231C 47C0 B401 207B BC02 6008 6BE8 211B 231C 47C0 B401 4800 E001 01C8 0000 BC02 6008 6BA8 211B 231C 47C0 6800 2120 2381 47C0 6BE8 211B 231C 47C0 6800 2100 2381 47C0 6830 B083 9601 466E 6030 2000 60B0 F000 F803 B500 F000 F809 F7FF FFFB 0031 2315 47C0 6876 B003 2383 47C0 2000 6228 2300 68BA 8213 2002 B401 6A28 F000 F808 2002 B401 6A28 3001 6228 2300 68BA 8213 BC02 4281 DA01 F000 F810 6BA8 B401 6A28 0080 BC02 5808 B401 2000 BC02 2302 47C0 2121 2381 47C0 F7FF FFE3 2000 6228 2300 68BA 8213 2002 B401 6A28 F000 F808 2002 B401 6A28 3001 6228 2300 68BA 8213 BC02 4281 DA01 F000 F810 6BE8 B401 6A28 0080 BC02 5808 B401 2000 BC02 2302 47C0 2121 2381 47C0 F7FF FFE3 2000 2101 2381 47C0 BD00 0104 0102 0000 0000 2E70 2000 2E50 2000 2E7C 2000 2E5C 2000
";
$carray=preg_split("/[\s]+/",trim($code));
preg_match('/[0-9a-f]{8}/i',array_shift($carray),$m);
$addr=hexdec($m[0]);

$libs="
#define LIB_CALC 0
#define LIB_CALC_FLOAT 1
#define LIB_HEX 2
#define LIB_ADD_STRING 3
#define LIB_STRNCMP 4
#define LIB_VAL 5
#define LIB_LEN 6
#define LIB_INT 7
#define LIB_RND 8
#define LIB_FLOAT 9
#define LIB_VAL_FLOAT 10
#define LIB_MATH 11
#define LIB_MID 12
#define LIB_CHR 13
#define LIB_DEC 14
#define LIB_FLOAT_STRING 15
#define LIB_SPRINTF 16
#define LIB_READ 17
#define LIB_CREAD 18
#define LIB_READ_STR 19
#define LIB_ASC 20
#define LIB_POST_GOSUB 21
#define LIB_DISPLAY_FUNCTION 22
#define LIB_INKEY 23
#define LIB_INPUT 24
#define LIB_DRAWCOUNT 25
#define LIB_KEYS 26
#define LIB_NEW 27
#define LIB_OBJ_FIELD 28
#define LIB_OBJ_METHOD 29
#define LIB_PRE_METHOD 30
#define LIB_POST_METHOD 31

#define LIB_DEBUG 128
#define LIB_PRINT 129
#define LIB_LET_STR 130
#define LIB_END 131
#define LIB_LINE_NUM 132
#define LIB_DIM 133
#define LIB_RESTORE 134
#define LIB_VAR_PUSH 135
#define LIB_VAR_POP 136
#define LIB_DISPLAY 137
#define LIB_WAIT 138
#define LIB_SET_DRAWCOUNT 139
#define LIB_STR_TO_OBJECT 140
#define LIB_DELETE 141
";
$larray=array();
preg_replace_callback('/(LIB_\S+)\s+([0-9]+)/',function($m){
		global $larray;
		$larray[intval($m[2])]=$m[1];
	},$libs);

for($i=0;$i<count($carray);$i++){
	$inst=hexdec($carray[$i]);
	$inst2=hexdec(@$carray[$i+1]);
	echo dechex($addr+$i*2),' '.$carray[$i],' ';
	switch($inst>>10){
		case 0b000000: $i=inst0000($i,$inst,$inst2); break;
		case 0b000001: $i=inst0400($i,$inst,$inst2); break;
		case 0b000010: $i=inst0800($i,$inst,$inst2); break;
		case 0b000011: $i=inst0C00($i,$inst,$inst2); break;
		case 0b000100: $i=inst1000($i,$inst,$inst2); break;
		case 0b000101: $i=inst1400($i,$inst,$inst2); break;
		case 0b000110: $i=inst1800($i,$inst,$inst2); break;
		case 0b000111: $i=inst1C00($i,$inst,$inst2); break;
		case 0b001000: $i=inst2000($i,$inst,$inst2); break;
		case 0b001001: $i=inst2400($i,$inst,$inst2); break;
		case 0b001010: $i=inst2800($i,$inst,$inst2); break;
		case 0b001011: $i=inst2C00($i,$inst,$inst2); break;
		case 0b001100: $i=inst3000($i,$inst,$inst2); break;
		case 0b001101: $i=inst3400($i,$inst,$inst2); break;
		case 0b001110: $i=inst3800($i,$inst,$inst2); break;
		case 0b001111: $i=inst3C00($i,$inst,$inst2); break;
		case 0b010000: $i=inst4000($i,$inst,$inst2); break;
		case 0b010001: $i=inst4400($i,$inst,$inst2); break;
		case 0b010010: $i=inst4800($i,$inst,$inst2,$carray); break;
		case 0b010011: $i=inst4C00($i,$inst,$inst2,$carray); break;
		case 0b010100: $i=inst5000($i,$inst,$inst2); break;
		case 0b010101: $i=inst5400($i,$inst,$inst2); break;
		case 0b010110: $i=inst5800($i,$inst,$inst2); break;
		case 0b010111: $i=inst5C00($i,$inst,$inst2); break;
		case 0b011000: $i=inst6000($i,$inst,$inst2); break;
		case 0b011001: $i=inst6400($i,$inst,$inst2); break;
		case 0b011010: $i=inst6800($i,$inst,$inst2); break;
		case 0b011011: $i=inst6C00($i,$inst,$inst2); break;
		case 0b011100: $i=inst7000($i,$inst,$inst2); break;
		case 0b011101: $i=inst7400($i,$inst,$inst2); break;
		case 0b011110: $i=inst7800($i,$inst,$inst2); break;
		case 0b011111: $i=inst7C00($i,$inst,$inst2); break;
		case 0b100000: $i=inst8000($i,$inst,$inst2); break;
		case 0b100001: $i=inst8400($i,$inst,$inst2); break;
		case 0b100010: $i=inst8800($i,$inst,$inst2); break;
		case 0b100011: $i=inst8C00($i,$inst,$inst2); break;
		case 0b100100: $i=inst9000($i,$inst,$inst2); break;
		case 0b100101: $i=inst9400($i,$inst,$inst2); break;
		case 0b100110: $i=inst9800($i,$inst,$inst2); break;
		case 0b100111: $i=inst9C00($i,$inst,$inst2); break;
		case 0b101000: $i=instA000($i,$inst,$inst2); break;
		case 0b101001: $i=instA400($i,$inst,$inst2); break;
		case 0b101010: $i=instA800($i,$inst,$inst2); break;
		case 0b101011: $i=instAC00($i,$inst,$inst2); break;
		case 0b101100: $i=instB000($i,$inst,$inst2); break;
		case 0b101101: $i=instB400($i,$inst,$inst2); break;
		case 0b101110: $i=instB800($i,$inst,$inst2); break;
		case 0b101111: $i=instBC00($i,$inst,$inst2); break;
		case 0b110000: $i=instC000($i,$inst,$inst2); break;
		case 0b110001: $i=instC400($i,$inst,$inst2); break;
		case 0b110010: $i=instC800($i,$inst,$inst2); break;
		case 0b110011: $i=instCC00($i,$inst,$inst2); break;
		case 0b110100: $i=instD000($i,$inst,$inst2); break;
		case 0b110101: $i=instD400($i,$inst,$inst2); break;
		case 0b110110: $i=instD800($i,$inst,$inst2); break;
		case 0b110111: $i=instDC00($i,$inst,$inst2); break;
		case 0b111000: $i=instE000($i,$inst,$inst2); break;
		case 0b111001: $i=instE400($i,$inst,$inst2); break;
		case 0b111010: $i=instE800($i,$inst,$inst2); break;
		case 0b111011: $i=instEC00($i,$inst,$inst2); break;
		case 0b111100: $i=instF000($i,$inst,$inst2,$addr); break;
		case 0b111101: $i=instF400($i,$inst,$inst2,$addr); break;
		case 0b111110: $i=instF800($i,$inst,$inst2); break;
		case 0b111111: $i=instFC00($i,$inst,$inst2); break;
		default:
			echo "\nUnknown error\n";
			exit;
	}
}

function inst0000($i,$inst,$inst2){
	switch(($inst>>6)&15){
		case 0b0000:
			echo "MOVS R",$inst&7,",R",($inst>>3)&7,"\n";
			break;
		default:
			echo "inst0000\n";
		break;
	}
	return $i;
}
function inst0400($i,$inst,$inst2){
	echo "inst0400\n";
	return $i;
}
function inst0800($i,$inst,$inst2){
	echo "inst0800\n";
	return $i;
}
function inst0C00($i,$inst,$inst2){
	echo "inst0C00\n";
	return $i;
}
function inst1000($i,$inst,$inst2){
	echo "inst1000\n";
	return $i;
}
function inst1400($i,$inst,$inst2){
	echo "inst1400\n";
	return $i;
}
function inst1800($i,$inst,$inst2){
	echo "inst1800\n";
	return $i;
}
function inst1C00($i,$inst,$inst2){
	echo "inst1C00\n";
	return $i;
}
function inst2000($i,$inst,$inst2){
	global $larray;
	//echo "inst2000\n";
	echo 'MOVS R',($inst>>8)&7,', #0x',dechex($inst&255);
	if (0x47c0==$inst2 && 3==(($inst>>8)&7)) echo ' (',$larray[$inst&255],')';
	echo "\n";
	return $i;
}
function inst2400($i,$inst,$inst2){
	echo "inst2400\n";
	return $i;
}
function inst2800($i,$inst,$inst2){
	echo "inst2800\n";
	return $i;
}
function inst2C00($i,$inst,$inst2){
	echo "inst2C00\n";
	return $i;
}
function inst3000($i,$inst,$inst2){
	// echo "inst3000\n";
	echo "ADDS R",($inst>>8)&7,",#",$inst&255,"\n";
	return $i;
}
function inst3400($i,$inst,$inst2){
	// echo "inst3400\n";
	return inst3000($i,$inst,$inst2);
}
function inst3800($i,$inst,$inst2){
	echo "inst3800\n";
	return $i;
}
function inst3C00($i,$inst,$inst2){
	echo "inst3C00\n";
	return $i;
}
function inst4000($i,$inst,$inst2){
	switch(($inst>>6)&15){
		case 0b1010:
			echo "CMP R",$inst&7,",R",($inst>>3)&7,"\n";
			break;
		default:
			echo "inst4000\n";
			break;
	}
	return $i;
}
function inst4400($i,$inst,$inst2){
	switch(($inst>>7)&7){
		case 0b111:
			echo 'BLX R',($inst>>3)&15,"\n";
			break;
		case 0b100:
		case 0b101:
			echo "MOV R",(($inst>>4)&8)+($inst&7),",R",($inst>>3)&15,"\n";
			break;
		default:
			echo "inst4400\n";
			break;
	}
	return $i;
}
function inst4800($i,$inst,$inst2,$carray){
	//echo "inst4800\n";
	switch($inst){
		case 0x4800:
			if ($inst2!=0xe001) break;
			for($j=1;$j<4;$j++) echo $carray[$i+$j],' ';
			echo 'LDR R',($inst>>8)&7,',0x',dechex(hexdec($carray[$i+2])|(hexdec($carray[$i+3])<<16)),"\n";
			return $i+3;
		case 0x4801:
			if ($inst2!=0xe002) break;
			for($j=1;$j<5;$j++) echo $carray[$i+$j],' ';
			echo 'LDR R',($inst>>8)&7,',0x',dechex(hexdec($carray[$i+3])|(hexdec($carray[$i+4])<<16)),"\n";
			return $i+4;
		default:
			break;
	}
	echo 'LDR R',($inst>>8)&7,',[PC+#',$inst&255,"]\n";
	return $i;
}
function inst4C00($i,$inst,$inst2,$carray){
	//echo "inst4C00\n";
	return inst4800($i,$inst,$inst2,$carray);
}
function inst5000($i,$inst,$inst2){
	echo "inst5000\n";
	return $i;
}
function inst5400($i,$inst,$inst2){
	echo "inst5400\n";
	return $i;
}
function inst5800($i,$inst,$inst2){
	echo "inst5800\n";
	return $i;
}
function inst5C00($i,$inst,$inst2){
	echo "inst5C00\n";
	return $i;
}
function inst6000($i,$inst,$inst2){
	//echo "inst6000\n";
	echo 'STR R',$inst&7,',[R',($inst>>3)&7,',#',($inst>>6)&31,"]\n";
	return $i;
}
function inst6400($i,$inst,$inst2){
	//echo "inst6400\n";
	return inst6000($i,$inst,$inst2);
}
function inst6800($i,$inst,$inst2){
	//echo "inst6800\n";
	echo 'LDR R',$inst&7,',[R',($inst>>3)&7,',#',($inst>>6)&31,"]\n";
	return $i;
}
function inst6C00($i,$inst,$inst2){
	//echo "inst6C00\n";
	return inst6800($i,$inst,$inst2);
}
function inst7000($i,$inst,$inst2){
	echo "inst7000\n";
	return $i;
}
function inst7400($i,$inst,$inst2){
	echo "inst7400\n";
	return $i;
}
function inst7800($i,$inst,$inst2){
	echo "inst7800\n";
	return $i;
}
function inst7C00($i,$inst,$inst2){
	echo "inst7C00\n";
	return $i;
}
function inst8000($i,$inst,$inst2){
	//echo "inst8000\n";
	echo 'STRH R',$inst&7,',[R',($inst>>3)&7,',#',($inst>>6)&31,"]\n";
	return $i;
}
function inst8400($i,$inst,$inst2){
	//echo "inst8400\n";
	return inst8000($i,$inst,$inst2);
}
function inst8800($i,$inst,$inst2){
	//echo "inst8800\n";
	echo 'LDRH R',$inst&7,',[R',($inst>>3)&7,',#',($inst>>6)&31,"]\n";
	return $i;
}
function inst8C00($i,$inst,$inst2){
	//echo "inst8C00\n";
	return inst8800($i,$inst,$inst2);
}
function inst9000($i,$inst,$inst2){
	//echo "inst9000\n";
	echo "STR R",($inst>>8)&7,",[SP,#",($inst&255)<<2,"]\n";
	return $i;
}
function inst9400($i,$inst,$inst2){
	//echo "inst9400\n";
	return inst9000($i,$inst,$inst2);
}
function inst9800($i,$inst,$inst2){
	echo "inst9800\n";
	return $i;
}
function inst9C00($i,$inst,$inst2){
	echo "inst9C00\n";
	return $i;
}
function instA000($i,$inst,$inst2){
	echo "instA000\n";
	return $i;
}
function instA400($i,$inst,$inst2){
	echo "instA400\n";
	return $i;
}
function instA800($i,$inst,$inst2){
	echo "instA800\n";
	return $i;
}
function instAC00($i,$inst,$inst2){
	echo "instAC00\n";
	return $i;
}
function instB000($i,$inst,$inst2){
	switch(($inst>>7)&3){
		case 0b001:
			echo "SUB SP,SP,#",($inst&7)<<2,"\n";
			break;
		case 0b000:
			echo "ADD SP,SP,#",($inst&7)<<2,"\n";
			break;
		default:
			echo "instB000\n";
			break;
	}
	return $i;
}
function instB400($i,$inst,$inst2){
	switch(($inst>>9)&3){
		case 0b10:
			echo 'PUSH {';
			if (($inst>>8)&1) echo 'LR,';
			for($j=0;$j<8;$j++){
				if (($inst>>$j)&1) echo 'R',$j,',';
			}
			echo "}\n";
			break;
		default:
			echo "instB400\n";
			break;
	}
	return $i;
}
function instB800($i,$inst,$inst2){
	echo "instB800\n";
	return $i;
}
function instBC00($i,$inst,$inst2){
	switch(($inst>>9)&3){
		case 0b10:
			echo 'POP {';
			if (($inst>>8)&1) echo 'PC,';
			for($j=0;$j<8;$j++){
				if (($inst>>$j)&1) echo 'R',$j,',';
			}
			echo "}\n";
			break;
		default:
			echo "instBC00\n";
			break;
	}
	return $i;
}
function instC000($i,$inst,$inst2){
	echo "instC000\n";
	return $i;
}
function instC400($i,$inst,$inst2){
	echo "instC400\n";
	return $i;
}
function instC800($i,$inst,$inst2){
	echo "instC800\n";
	return $i;
}
function instCC00($i,$inst,$inst2){
	echo "instCC00\n";
	return $i;
}
function instD000($i,$inst,$inst2){
	switch(($inst>>8)&15){
		case 0b0000: echo "BEQ "; break;
		case 0b0001: echo "BNE "; break;
		case 0b0010: echo "BCS "; break;
		case 0b0011: echo "BCC "; break;
		case 0b0100: echo "BMI "; break;
		case 0b0101: echo "BPL "; break;
		case 0b0110: echo "BVS "; break;
		case 0b0111: echo "BVC "; break;
		case 0b1000: echo "BHI "; break;
		case 0b1001: echo "BLS "; break;
		case 0b1010: echo "BGE "; break;
		case 0b1011: echo "BLT "; break;
		case 0b1100: echo "BGT "; break;
		case 0b1101: echo "BLE "; break;
		case 0b1110: echo "B "; break;
		case 0b1111: 
			echo "SVC #",$inst&255;
			return $i;
		default:
			echo "instD000\n";
			return $i;
	}
	if ($inst&0x80) {
		echo ($inst&255)-255,"\n";
	} else {
		echo $inst&255,"\n";
	}
	return $i;
}
function instD400($i,$inst,$inst2){
	// echo "instD400\n";
	return instD000($i,$inst,$inst2);
}
function instD800($i,$inst,$inst2){
	// echo "instD800\n";
	return instD000($i,$inst,$inst2);
}
function instDC00($i,$inst,$inst2){
	// echo "instDC00\n";
	return instD000($i,$inst,$inst2);
}
function instE000($i,$inst,$inst2){
	//echo "instE000\n";
	echo 'B ',($inst&0x7ff)*2,"\n";
	return $i;
}
function instE400($i,$inst,$inst2){
	//echo "instE400\n";
	return instE000($i,$inst,$inst2);
}
function instE800($i,$inst,$inst2){
	echo "instE800\n";
	return $i;
}
function instEC00($i,$inst,$inst2){
	echo "instEC00\n";
	return $i;
}
function instF000($i,$inst,$inst2,$addr){
	echo substr('000'.strtoupper(dechex($inst2)),-4),' ';
	if ((0xf800 & $inst2)!=0xf800) exit("Unsupported");
	//echo "instF000\n";
	$jump=(($inst&0x07ff)<<11) | (($inst2&0x07ff)<<0);
	if ($jump&(1<<21)) {
		echo 'BL -',(1<<22)-$jump;
	} else {
		echo 'BL +',$jump;
	}
	echo ' (',dechex($addr+($i+2+$jump)*2),")\n";
	return $i+1;
}
function instF400($i,$inst,$inst2,$addr){
	// echo "instF400\n";
	return instF000($i,$inst,$inst2,$addr);
}
function instF800($i,$inst,$inst2){
	echo "instF800\n";
	return $i;
}
function instFC00($i,$inst,$inst2){
	echo "instFC00\n";
	return $i;
}
