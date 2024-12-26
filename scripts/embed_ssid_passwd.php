<?php

date_default_timezone_set('America/Los_Angeles');

if (file_exists('phyllosoma_kb.uf2')) define('FILENAME','phyllosoma_kb.uf2');
elseif (file_exists('puerulus_kb.uf2')) define('FILENAME','puerulus_kb.uf2');
elseif (file_exists('phyllosoma.uf2')) define('FILENAME','phyllosoma.uf2');
elseif (file_exists('puerulus.uf2')) define('FILENAME','puerulus.uf2');
else exit("The *.uf2 file donesn't exist");

define('WIFI_SSID','your Wi-Fi SSID here');
define('WIFI_PASSWD','your Wi-Fi password here');

// Initializations
$phyllosoma=@file_get_contents('./'.FILENAME);
if (!@strlen($phyllosoma)) exit(FILENAME.' not found in current directory');
$embedded=$phyllosoma;
$embedded_ssid=str_repeat("MACHIKANIA_DEFAULT_WIFI_SSID\0\0\0\0",4);
$embedded_passwd=str_repeat("MACHIKANIA_DEFAULT_WIFI_PASSWD\0\0",4);

// Check uf2 file
echo "Investigating uf2 file...\n";
if (!strpos_ex($phyllosoma,$embedded_ssid)) exit ("MACHIKANIA_DEFAULT_WIFI_SSID not found");
echo "MACHIKANIA_DEFAULT_WIFI_SSID found\n";
if (!strpos_ex($phyllosoma,$embedded_passwd)) exit ("MACHIKANIA_DEFAULT_WIFI_PASSWD not found");
echo "MACHIKANIA_DEFAULT_WIFI_PASSWD found\n\n";

// Replace
replace_word(WIFI_SSID,$embedded_ssid);
replace_word(WIFI_PASSWD,$embedded_passwd);

// All done. Save the file
file_put_contents('./result.uf2',$embedded);
echo "\ndone! result.uf2 was created.";

function replace_word($new_word,$embeddedname){
	global $phyllosoma,$embedded;
	$new_word.="\0";
	// Find the position
	$objpos=strpos_ex($phyllosoma,$embeddedname);
	if (!$objpos) exit ("Unknown error!");
	$result=substr($embedded,0,$objpos);
	// Replace
	for($strp=0;$strp<strlen($new_word);$strp++){
		if (substr($phyllosoma,$objpos,1)!=substr($embeddedname,$strp,1)) {
			// Skip 256 bytes
			$result.=substr($embedded,$objpos,256);
			$objpos+=256;
			if (substr($phyllosoma,$objpos,1)!=substr($embeddedname,$strp,1)) exit('Unexpected error!');
		}
		$result.=substr($new_word,$strp,1);
		$objpos++;
	}
	// Add remaining region
	$result.=substr($embedded,$objpos);
	// Replace
	$embedded=$result;
}

function strpos_ex($str,$needle){
	$query='/(';
	$query.=preg_quote($needle,'/');
	for($i=1;$i<strlen($needle);$i++){
		$query.='|';
		$query.=preg_quote(substr($needle,0,$i),'/');
		$query.='[\s\S]{256}';
		$query.=preg_quote(substr($needle,$i),'/');
	}
	$query.=')/';
	if (!preg_match($query,$str,$m)) return false;
	return strpos($str,$m[0]);
}
