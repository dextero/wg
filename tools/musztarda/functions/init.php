<?php
#wykrywanie skad user przyszedl
	if ($_GET['from'] != '')
	{
		$from = $_GET['from'];
		$from = substr($from, 0, 12);
		$from = preg_replace('/[^a-zA-Z0-9()]/', '', $from);		
	}
	else
		$from = 'unknown';
	
#wykrywanie jezyka, domyslny angielski:
	$lang = 'en';
	if (isset($_GET['lang'])) {
		switch($_GET['lang']) {
			case 'en': $lang='en'; break;
			case 'pl': $lang='pl'; break;
		}
	}

#stale ;-P	
	$gameVersion = "1.1";
	
	switch ($lang) {
		case 'en' : $gameTitle = "Warlock's Gauntlet"; break;
		case 'pl' : $gameTitle = "Warsztat Game"; break;
	}
?>
