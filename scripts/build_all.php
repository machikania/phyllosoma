<?php

// Path configurations. Execute cmake in each build directory before this script
define('PICO1_BUILD_DIR', '../build_pico1/');
define('PICO2_BUILD_DIR', '../build_pico2/');
define('PICO1W_BUILD_DIR', '../build_pico1w/');
define('PICO2W_BUILD_DIR', '../build_pico2w/');
define('CONFIG_CMAKE_PATH','../phyllosoma/config.cmake');
define('MAIN_C_PATH','../phyllosoma/main.c');
//define('MAKE_COMMAND','start /wait mingw32-make');
define('MAKE_COMMAND','mingw32-make');

// UF2 files to be built
$fu2s=array();
// pico_ili9341
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_ili9341/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_ili9341/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_ili9341/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_ili9341/embed/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_w_ili9341/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_w_ili9341/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_w_ili9341/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico_w_ili9341/embed/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_ili9341/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_ili9341/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_ili9341/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_ili9341/embed/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_w_ili9341/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_w_ili9341/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_w_ili9341/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>1,'build'=>'pico_ili9341','uf2'=>'machikania-p/pico2_w_ili9341/embed/phyllosoma_kb.uf2');
// pico_ili9488
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_ili9488/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_ili9488/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_ili9488/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_ili9488/embed/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_w_ili9488/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_w_ili9488/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_w_ili9488/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico_w_ili9488/embed/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_ili9488/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_ili9488/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_ili9488/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_ili9488/embed/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_w_ili9488/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_w_ili9488/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_w_ili9488/embed/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>1,'build'=>'pico_ili9488','uf2'=>'machikania-p2/pico2_w_ili9488/embed/phyllosoma_kb.uf2');
// pico_picocalc
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_picocalc','uf2'=>'machikania-pc/pico_picocalc/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_picocalc','uf2'=>'machikania-pc/pico_w_picocalc/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_picocalc','uf2'=>'machikania-pc/pico2_picocalc/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_picocalc','uf2'=>'machikania-pc/pico2_w_picocalc/phyllosoma_kb.uf2');
// pico_ntsc
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_ntsc/puerulus.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_ntsc/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_ntsc/embed/puerulus.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_ntsc/embed/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_w_ntsc/puerulus.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_w_ntsc/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_w_ntsc/embed/puerulus.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico_w_ntsc/embed/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_ntsc/puerulus.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_ntsc/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_ntsc/embed/puerulus.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_ntsc/embed/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_w_ntsc/puerulus.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_w_ntsc/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_w_ntsc/embed/puerulus.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>1,'build'=>'pico_ntsc','uf2'=>'machikania-pu/pico2_w_ntsc/embed/puerulus_kb.uf2');
// xiao_ntsc
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_ntsc/puerulus.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_ntsc/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_ntsc/embed/puerulus.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>1,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_ntsc/embed/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_rp2350_ntsc/puerulus.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_rp2350_ntsc/puerulus_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_rp2350_ntsc/embed/puerulus.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>1,'build'=>'xiao_ntsc','uf2'=>'machikania-pu/xiao_rp2350_ntsc/embed/puerulus_kb.uf2');
// rp2350_lcd_1_47
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'rp2350_lcd_1_47','uf2'=>'machikania-rl/rp2350_lcd_1_47/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'rp2350_lcd_1_47','uf2'=>'machikania-rl/rp2350_lcd_1_47/phyllosoma_kb.uf2');
// rp2350_lcd_2
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'rp2350_lcd_2','uf2'=>'machikania-rl2/rp2350_lcd_2/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'rp2350_lcd_2','uf2'=>'machikania-rl2/rp2350_lcd_2/phyllosoma_kb.uf2');
// pico_restouch
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico_restouch/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico_restouch/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico_w_restouch/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico1w','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico_w_restouch/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico2_restouch/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico2_restouch/phyllosoma_kb.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico2_w_restouch/phyllosoma.uf2');
$uf2s[]=array('pico'=>'pico2w','debug'=>0,'build'=>'pico_restouch','uf2'=>'machikania-rt/pico2_w_restouch/phyllosoma_kb.uf2');

// Initiations
$curdir=getcwd();
$config_cmake=file_get_contents(CONFIG_CMAKE_PATH);
$config_cmake=preg_replace('/#{32}[\r\n]+[\s\S]*(#{32}[\r\n]+# Do not touch the lines below #[\r\n]+#{32})/','$1',$config_cmake);
$pico_build_dirs=array();
$pico_build_dirs['pico1']=PICO1_BUILD_DIR;
$pico_build_dirs['pico2']=PICO2_BUILD_DIR;
$pico_build_dirs['pico1w']=PICO1W_BUILD_DIR;
$pico_build_dirs['pico2w']=PICO2W_BUILD_DIR;
$log='';

// Create destination directories and delete uf2 files if exist
for($i=0;$i<count($uf2s);$i++){
	$dir=$uf2s[$i]['uf2'];
	$dir=preg_replace('/\/[^\/]*$/','/',$dir);
	while($dir!=""){
		$newd=preg_replace('/\/.*/','',$dir);
		//echo $newd,"\n";
		@mkdir($newd);
		chdir($newd);
		$dir=substr($dir,strlen($newd)+1);
	}
	chdir($curdir);
}

// Build UF2 files
$pico=$debug=$build='not set';
for($i=0;$i<count($uf2s);$i++){
	// Logs
	$uf2=$uf2s[$i]['uf2'];
	echo '***** Build '.($i+1).' of '.count($uf2s)." $uf2 *****\n";
	$log.='***** Build '.($i+1).' of '.count($uf2s)." $uf2 *****\n";
	// Check uf2 file
	$uf2fn=preg_replace('/^.*\/([^\/]+)$/','$1',$uf2);
	if (file_exists($uf2)) continue;
	// Build
	if ($pico===$uf2s[$i]['pico'] && $debug===$uf2s[$i]['debug'] && $build===$uf2s[$i]['build']){
		// Build not required
		chdir($pico_build_dirs[$pico]);
	} else {
		// Build uf2 files
		$pico=$uf2s[$i]['pico'];
		$debug=$uf2s[$i]['debug'];
		$build=$uf2s[$i]['build'];
		chdir($curdir);
		// Refresh main.c to create uf2 files always
		$binary=file_get_contents(MAIN_C_PATH);
		file_put_contents(MAIN_C_PATH,$binary);
		// Modify config.cmake
		$set1='set(MACHIKANIA_BUILD '.$build.')';
		$set2='';
		if ($debug) $set2='set(MACHIKANIA_DEBUG_MODE 1)'; else $set2='';
		$cmake=preg_replace('/(#{32})([\r\n]+)# Do not touch the lines below #([\r\n]+)#{32}/','$1$2'.$set1.'$3'.$set2.'$3$0',$config_cmake);
		file_put_contents(CONFIG_CMAKE_PATH,$cmake);
		// Make
		chdir($pico_build_dirs[$pico]);
		$log.=shell_exec(MAKE_COMMAND);
		// Restore config.cmake
		file_put_contents(CONFIG_CMAKE_PATH,$config_cmake);
	}
	// Copy uf2 file
	if (!file_exists($uf2fn)) {
		echo "$uf2fn not found\n";
		$log.="$uf2fn not found\n";
		break;
	}
	if (filemtime($uf2fn)<time()-600) {
		echo "$uf2fn is too old\n";
		$log.="$uf2fn is too old\n";
		break;
	}
	$binary=file_get_contents($uf2fn);
	chdir($curdir);
	file_put_contents($uf2,$binary);
	file_put_contents('log.txt',$log);
}
// All done
chdir($curdir);
file_put_contents('log.txt',$log);
