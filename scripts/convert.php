<?php

date_default_timezone_set('America/Los_Angeles');

// Initializations
$phyllosoma=@file_get_contents("./phyllosoma.uf2");
if (!@strlen($phyllosoma)) exit("phyllosoma.uf2 not found in current directory");
$embedded=$phyllosoma;
$maxlengths=array();

// Check uf2 file
echo "Investigating uf2 file...\n";
$maxlengths[0]=check_text_area('MACHIKAP');
for($classnum=1;$classnum<=0xf;$classnum++){
	$maxlengths[$classnum]=check_text_area('CLASS00'.strtoupper(dechex($classnum)));
	if (!$maxlengths[$classnum]) {
		$classnum--;
		break; 
	}
}

// Check BAS files in current directory
echo "\nInvestigating BASIC files in current directory...\n";
$d=glob('*.{BAS,INI,TXT,bas,ini,txt}',GLOB_BRACE);
$basnum=0;
if (0==count($d)) echo "No file to embed found\n";
for($i=0;$i<count($d);$i++) {
	if (12<strlen($d[$i])) exit("$d[$i]: Too long file name!");
	$flen=filesize($d[$i]);
	echo substr($d[$i].'       ',0,12),' found: '."$flen bytes ";
	if (strtoupper($d[$i])=='MACHIKAP.BAS') {
		if ($maxlengths[0]<$flen) exit(' file too large!');
		else echo "(fits to $maxlengths[0] bytes area; MACHIKAP)\n";
		// Replace text file
		replace_bas($d[$i],'MACHIKAP');
	} else {
		$basnum++;
		if ($classnum<$basnum) exit('Too many class files!');
		if ($maxlengths[$basnum]<$flen) exit(' file too large!');
		else echo "(fits to $maxlengths[$basnum] bytes area; CLASS00".strtoupper(dechex($basnum)).")\n";
		// Replace file name
		replace_filename(strtoupper($d[$i]),'CLASS00'.strtoupper(dechex($basnum)).'.BAS');
		// Replace text file
		replace_bas($d[$i],'CLASS00'.strtoupper(dechex($basnum)));
	}
}

// All done. Save the file
file_put_contents('./result.uf2',$embedded);
echo "\ndone! result.uf2 was created.";

function check_text_area($fname){
	global $phyllosoma;
	// Check BAS file name, first
	if (!strpos_ex($phyllosoma,$fname.'.BAS')) return 0;
	echo "$fname.BAS found: ";
	// Check length of text main area
	$low=0;
	$high=0xfff;
	// Search of last line
	while($low<$high-1){
		$midpoint=($low+$high)>>1;
		$line="REM $fname".substr('00'.strtoupper(dechex($midpoint)),-3);
		$line.="\n";
		$pos=strpos_ex($phyllosoma,$line);
		if ($pos!==false) {
			// Found
			$low=$midpoint;
		} else {
			// Not found
			$high=$midpoint-1;
		}
	}
	$high=($high+1)*16;
	echo "maximum length: $high bytes\n";
	return $high;
}

function replace_filename($filename,$embeddedname){
	global $phyllosoma,$embedded;
	$filename.="\0";
	$embeddedname.="\0";
	// Find the position
	$objpos=strpos_ex($phyllosoma,$embeddedname);
	if (!$objpos) exit ("Filename $embeddedname not found!");
	$result=substr($embedded,0,$objpos);
	// Replace
	for($strp=0;$strp<strlen($filename);$strp++){
		if (substr($phyllosoma,$objpos,1)!=substr($embeddedname,$strp,1)) {
			// Skip 256 bytes
			$result.=substr($embedded,$objpos,256);
			$objpos+=256;
			if (substr($phyllosoma,$objpos,1)!=substr($embeddedname,$strp,1)) exit('Unexpected error!');
		}
		$result.=substr($filename,$strp,1);
		$objpos++;
	}
	// Add remaining region
	$result.=substr($embedded,$objpos);
	// Replace
	$embedded=$result;
}

function replace_bas($filename,$embeddedname){
	global $phyllosoma,$embedded;
	$file=file_get_contents($filename)."\0";
	$filepos=0;
	$filelen=strlen($file);
	// Find the first position
	$str='REM '.$embeddedname."000\n";
	$objpos=strpos_ex($phyllosoma,$str);
	if (!$objpos) exit ("Beginning of $embeddedname not found!");
	$result=substr($embedded,0,$objpos);
	// Replace
	while($filepos<$filelen){
		$str='REM '.$embeddedname.substr('000'.strtoupper(dechex($filepos>>4)),-3)."\n";
		for($strp=0;$strp<16;$strp++){
			if (substr($phyllosoma,$objpos,1)!=substr($str,$strp,1)) {
				// Skip 256 bytes
				$result.=substr($embedded,$objpos,256);
				$objpos+=256;
				if (substr($phyllosoma,$objpos,1)!=substr($str,$strp,1)) exit('Unexpected error!');
			}
			$result.=substr($file,$filepos++,1);
			$objpos++;
			if ($filelen<=$filepos) break;
		}
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
