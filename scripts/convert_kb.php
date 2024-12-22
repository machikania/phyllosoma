<?php

	if (file_exists('phyllosoma_kb.uf2')) define('FILENAME','phyllosoma_kb.uf2');
	elseif (file_exists('puerulus_kb.uf2')) define('FILENAME','puerulus_kb.uf2');
	else exit("The *_kb.uf2 file donesn't exist");

define('FILENAME','phyllosoma_kb.uf2');
require('./convert.php');
