<?php

function logMessage($message) {
	$file = fopen('log','a');
	if (!$file)
		return;
	
	fwrite($file, date("F j, Y, g:ia ").$_SERVER['REMOTE_ADDR'].": ".$message."\n");
	fclose($file);
}
	
?>