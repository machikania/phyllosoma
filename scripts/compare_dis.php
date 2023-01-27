<?php

date_default_timezone_set('America/Los_Angeles');

$dis1=file_get_contents('./phyllosoma_kb1.dis');
$dis2=file_get_contents('./phyllosoma_kb2.dis');

$dis1=preg_split('/(\r\n|\r|\n)/',$dis1);
$dis2=preg_split('/(\r\n|\r|\n)/',$dis2);

$len=count($dis1);
if (count($dis2)<$len) $len=count($dis2);

$result='';
for($i=0;$i<$len;$i++){
	$line1=$dis1[$i];
	$line2=$dis2[$i];
	if ($line1==$line2) continue;
	//echo $i+1,"\n";
	$linelen=strlen($line1);
	if ($linelen!=strlen($line2)) {
		showdiff($line1,$line2,$i);
		continue;
	}
	for($j=0;$j<$linelen;$j++){
		$c1=ord($line1[$j]);
		$c2=ord($line2[$j]);
		if ($c1==$c2) continue;
		if ((0x30<=$c1 && $c1<=0x39 || 0x61<=$c1 && $c1<=0x66) && (0x30<=$c2 && $c2<=0x39 || 0x61<=$c2 && $c2<=0x66)) continue;
		showdiff($line1,$line2,$i);
		break;
	}
}

file_put_contents('./result.txt',$result);

function showdiff($line1,$line2,$i){
	global $result;
	$i++;
	$result.="----- line: $i -----\n";
	$result.=$line1."\n";
	$result.=$line2."\n";
}
