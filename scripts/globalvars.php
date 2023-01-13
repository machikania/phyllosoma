<?php

$map=file_get_contents('./build/phyllosoma_kb.elf.map');

$result=array();

preg_match_all('/\.data\.([^\.\s]*).*[\r\n]+\s+0x[0-9a-f]+.*\/([^\/]+\.c)\.obj/i',$map,$m);
for($i=0;$i<count($m[0]);$i++){
	echo $m[1][$i],' ',$m[2][$i],"\n";
	if (!isset($result[$m[2][$i]])) $result[$m[2][$i]]=array();
	$result[$m[2][$i]][]=$m[1][$i];
}

echo "-----\n";

preg_match_all('/\.bss\.([^\.\s]+).*[\r\n]?.*CMakeFiles\/[^\/]*\.(?:dir|dir\/interface)\/([^\/]+\.c)\.obj/',$map,$m);
for($i=0;$i<count($m[0]);$i++){
	echo $m[1][$i],' ',$m[2][$i],"\n";
	if (!isset($result[$m[2][$i]])) $result[$m[2][$i]]=array();
	$result[$m[2][$i]][]=$m[1][$i];
}

$t='';
foreach($result as $file=>$vars){
	$t.="\n-----$file-----\n";
	for($i=0;$i<count($vars);$i++) $t.=$vars[$i]."\n";
}

file_put_contents('./result.txt',$t);
