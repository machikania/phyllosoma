<?php

date_default_timezone_set('America/Los_Angeles');

$result=array();

foreach(glob('../*.c') as $fname){
	foreach(checkFile($fname) as $words) $result[$words]=true;
}
foreach(glob('../wifi/*.c') as $fname){
	foreach(checkFile($fname) as $words) $result[$words]=true;
}
foreach(glob('./*.html') as $fname){
	foreach(checkFile($fname) as $words) $result[$words]=true;
}
ksort($result);
//print_r($result);

$text='';
foreach($result as $word=>$dummy) $text.="\t'$word',\n";

echo $text;
file_put_contents('./reserved_names.txt',$text);

function checkFile($fname){
	echo $fname,"\n";
	$t=file_get_contents($fname);
	if (substr($fname,-2)=='.c') {
		if (preg_match_all('/instruction_is\("([A-Z0-9]+)/',$t,$m)) return $m[1];
		else return array();
	} else if (substr($fname,-5)=='.html') {
		if (preg_match_all("/\t'([A-Z0-9]+)',/",$t,$m)) return $m[1];
		else return array();
	} else {
		echo "Unknown error";
		exit;
	}
}
