<?php

$version=array(
	"Phyllosoma",
	"1.4.1.0",
	"KM-1506",
);

$configs=array(
	'./config/pico_ili9341.h (embed)',
	'./config/pico_w_ili9341.h (embed)',
	'./config/pico_ili9488.h (embed)',
	'./config/pico_w_ili9488.h (embed)',
	'./config/xiao_embed.h (embed)',
	'./config/pico_ili9341.h',
	'./config/pico_w_ili9341.h',
	'./config/pico_ili9488.h',
	'./config/pico_w_ili9488.h',
);

check_dir('.','.');

function check_dir($dir,$fullpath){
	//echo $fullpath,"\n";
	chdir($dir);
	foreach(glob('*',GLOB_ONLYDIR) as $newdir){
		check_dir($newdir,$fullpath.'/'.$newdir);
	}
	foreach(glob('*.uf2') as $uf2filename){
		echo '=======',$fullpath,'/'.$uf2filename,"\n";
		check_uf2($uf2filename,$fullpath);
	}
	chdir('..');
}

function check_uf2($filename,$fullpath){
	global $configs,$version;
	$uf2file=file_get_contents($filename);
	// Check version
	for($i=0;$i<count($version);$i++){
		if (!strpos_ex($uf2file,$version[$i])) {
			echo "\n***** Version doesn't fit! It must be $version[$i] *****\n\n";
			return;
		}
		echo $version[$i],' ';
	}
	echo "OK\n";
	// Check config
	if (!preg_match('@/([^/]*)@',$fullpath,$m)) exit ("Error ".__LINE__);
	$config_file=$m[1];
	$embed_file=preg_match('@(/embed|xiao_embed|xiao_rp2350_embed)@',$fullpath);
	foreach($configs as $config){
		if (!strpos_ex($uf2file,$config)) continue;
		echo $config,"\n";
		if (!preg_match('@config/([^\.]*)@',$config,$m)) exit ("Error ".__LINE__);
		if ($m[1]==$config_file) echo "  config: OK";
		elseif ($m[1]==str_replace('pico2_','pico_',$config_file)) echo "  config: OK";
		elseif ($m[1]==str_replace('_rp2350_','_',$config_file)) echo "  config: OK";
		else echo "  config; NG";
		$embed=preg_match('@\(embed\)@',$config);
		if ($embed==$embed_file) echo "  embed state: OK";
		else echo "  embed state: NG";
		if (preg_match('/(pico2_|rp2350)/',$config_file) && substr($uf2file,0x1c,1)=='W') echo "  pico1/pico2 state: OK";
		elseif (preg_match('/(pico2_|rp2350)/',$config_file)==0 && substr($uf2file,0x1c,1)=='V') echo "  pico1/pico2 state: OK";
		else echo "  pico1/pico2 state: NG";
		echo "\n\n";
		return;
	}
	echo "***** Config header not found! *****\n\n";
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
