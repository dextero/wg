<?php

class Package {
    public $lang;
	public $platform;
	public $title;
	public $url;
	
    function __construct($args) {
		$this->lang = $args[0];
		$this->platform = $args[1];
		$this->title = $args[2];
		$this->url = $args[3];
	}
}

function getDownloadsMatching($lang, $platform, $title) {
	$lines = file("downloads.txt");
	$ret = array();
	foreach(file("downloads.txt") as $line) {
		$line = trim($line);
		if (empty($line)) continue;
		if ($line[0] == '#') continue;
		$line = preg_replace("/ {2,}/", " ", $line); //usuwanie podwojnych spacji
		$params = explode(" ", $line);
		if (sizeof($params) < 4) continue;
		
		if ($lang != null && $lang != $params[0]) continue;
		if ($platform != null && $platform != $params[1]) continue;
		if ($title != null && $title != $params[2]) continue;
		
		$ret[] = new Package($params);			
	}
	return $ret;
}
	
?>