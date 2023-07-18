<?php

ob_start();

$ram=file_get_contents('./MEMDUMP.BIN');
$map_file=file_get_contents('./phyllosoma.elf.map');
$compilerh=file_get_contents('../phyllosoma/compiler.h');

if (!(strlen($ram)&&strlen($map_file)&&strlen($compilerh))) exit;

// Get variables data
$vars=array();
$vardata=array();
foreach(array(
			'object',
			'kmbasic_variables',
			'kmbasic_data',
			'g_heap_begin',
			'g_class_list',
			'g_class_id_list',
			'g_empty_object_list',
			'g_cmpdata',
			'g_cmpdata_end',
		) as $var_name){
	$vars[$var_name]=variable_address($var_name);
	$vardata[$var_name]=int_data($vars[$var_name]);
}
echo 'Variable addresses: ';
print_r($vars);
echo 'Variable data: ';
print_r($vardata);

function variable_address($var_name){
	global $map_file;
	if (!preg_match('/\.'.$var_name.'\r?\n?[\s^\r\n]+0x([0-9a-fA-F]{8}).*\.obj\)?[\r\n]/',$map_file,$m)) exit('Error at '.__LINE__);
	return $m[1];
}

function int_data($addr){
	global $ram;
	if (is_string($addr)) $addr=hexdec($addr);
	$addr-=0x20000000;
	$ret=ord($ram[$addr]);
	$ret|=ord($ram[$addr+1])<<8;
	$ret|=ord($ram[$addr+2])<<16;
	$ret|=ord($ram[$addr+3])<<24;
	return dechex($ret);
}

function short_data($addr){
	global $ram;
	if (is_string($addr)) $addr=hexdec($addr);
	$addr-=0x20000000;
	$ret=ord($ram[$addr]);
	$ret|=ord($ram[$addr+1])<<8;
	return dechex($ret);
}

// Classes
echo "\n***** Class information follows *****\n";
$g_class_id_list=hexdec($vardata['g_class_id_list']);
echo 'g_class_id_list:     0x'.$vardata['g_class_id_list']."\n";
$g_class_list=hexdec($vardata['g_class_list']);
echo 'g_class_list:        0x'.$vardata['g_class_list']."\n";
$g_empty_object_list=hexdec($vardata['g_empty_object_list']);
echo 'g_empty_object_list: 0x'.$vardata['g_empty_object_list']."\n";
for($i=0;$i<10;$i++){
	$d=short_data($g_class_id_list+$i*2);
	if ('0'==$d) break;
	$class_struct=int_data($g_class_list+$i*4);
	$num=hexdec(int_data($class_struct));
	$empty_object=int_data($g_empty_object_list+$i*4);
	echo 'Class id: 0x'.$d.', structure at 0x'.$class_struct.' (length='.$num.'), empty object at 0x'.$empty_object."\n";
	echo "  class structure  empty object\n";
	for($j=0;$j<$num;$j++){
		$csd=int_data(hexdec($class_struct)+$j*4);
		$eod=int_data(hexdec($empty_object)+$j*4);
		echo '     '.substr('       0x'.$csd,-10);
		echo '     '.substr('       0x'.$eod,-10);
		$csd=hexdec($csd);
		echo '   ';
		if ($csd&0x00100000) echo ' public';
		if ($csd&0x00200000) echo ' static';
		if ($csd&0x00010000) echo ' method';
		if ($csd&0x00020000) echo ' field ';
		echo "\n";
	}
}

// CMPDATA
echo "\n***** CMPDATA *****\n";
$cmplist=array();
foreach(preg_split('/[\r\n]+/',$compilerh) as $line){
	if (!preg_match('/^#define\s+(CMPDATA_[\S]+)[\s]*0x([0-9A-F]+)/',$line,$m)) continue;
	//echo "$m[1] $m[2]\n";
	$cmplist[hexdec($m[2])]=$m[1];
}
print_r($cmplist);
$g_cmpdata=$vardata['g_cmpdata'];
$g_cmpdata_end=$vardata['g_cmpdata_end'];
//echo "$g_cmpdata $g_cmpdata_end \n";
$g_cmpdata_point=$g_cmpdata;
$g_cmpdata_end=hexdec($g_cmpdata_end);
$g_cmpdata_point=hexdec($g_cmpdata_point);
$cmparray=array();
while($g_cmpdata_point<$g_cmpdata_end){
	$cmpdata=int_data($g_cmpdata_point);
	$type=(hexdec($cmpdata)>>24)&255;
	$len=(hexdec($cmpdata)>>16)&255;
	$data16=hexdec($cmpdata)&255;
	//echo $cmpdata,"\n";
	$type=@$cmplist[$type]."($type)";
	if (!isset($cmparray[$type])) $cmparray[$type]=array();
	$t='at '.dechex($g_cmpdata_point).': 0x'.$cmpdata;
	for($i=1;$i<$len;$i++) $t.=' 0x'.int_data($g_cmpdata_point+$i*4);
	$cmparray[$type][]=$t;
	$g_cmpdata_point+=4*$len;
}
print_r($cmparray);

// Disassemble
$carray=array();
$addr=0x20000000;
for($a=$addr;$a<hexdec($vardata['object']);$a+=2){
	$c=ord($ram[$a-0x20000000])+(ord($ram[$a-0x20000000+1])<<8);
	$carray[]=strtoupper(substr('000'.dechex($c),-4));
}
//print_r($carray);
echo "\n***** Dissassembly *****\n";
require('./disassembler.php');

// All done
// Save the result
file_put_contents('./result.txt',ob_get_flush());
