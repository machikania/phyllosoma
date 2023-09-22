<?php

$libs=file_get_contents('../phyllosoma/compiler.h');

if (!isset($carray)) {
	$code="
	kmbasic_object:20003964
	F000 F812 2383 47C0 0003 0000 001A 1A02 001B 1B02 001C 1C02 396C 2000 0000 0000 0000 0000 0000 0000 4678 3002 E001 4B4F 0000 2101 2381 47C0 2383 47C0 2383 47C0 0103 0000 396C 2000 397C 2000
	";
	$carray=preg_split("/[\s]+/",trim($code));
	preg_match('/[0-9a-f]{8}/i',array_shift($carray),$m);
	$addr=hexdec($m[0]);
}

if (!$libs) exit;
$larray=array();
preg_replace_callback('/\n#define\s+(LIB_\S+)\s+([0-9]+)/',function($m){
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
	//echo "inst1C00\n";
	echo 'MOVS R',$inst&7,',R',($inst>>3)&7,',#',($inst>>6)&7,"\n";
	return $i;
}
function inst2000($i,$inst,$inst2){
	global $larray;
	//echo "inst2000\n";
	echo 'MOVS R',($inst>>8)&7,', #0x',dechex($inst&255);
	if (0x47c0==$inst2 && 3==(($inst>>8)&7)) echo ' (',@$larray[$inst&255],')';
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
	//echo "inst9800\n";
	echo "LDR R",($inst>>8)&7,",[SP,#",($inst&255)<<2,"]\n";
	return $i;
}
function inst9C00($i,$inst,$inst2){
	//echo "inst9C00\n";
	return inst9800($i,$inst,$inst2);
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
		echo ' (',dechex($addr+($i+2+$jump-(1<<22))*2),")\n";
	} else {
		echo 'BL +',$jump;
		echo ' (',dechex($addr+($i+2+$jump)*2),")\n";
	}
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
