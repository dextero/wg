<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html  xmlns="http://www.w3.org/1999/xhtml">
<head>
<?php
switch ($lang) {
    case 'en' : $pageTitle = "Warlock's Gauntlet Download - Public Tests"; break;
    case 'pl' : $pageTitle = "Warsztat Game Download - Testy Otwarte"; break;
}
print("<title>".$pageTitle."</title>\n");
?>
<meta content="text/html; charset=utf-8" http-equiv="content-type" />
<link rel="stylesheet" href="s.css" />
</head>
<body>
<div id="all">
	<div id="header">
<?php

	switch ($lang) {
		case 'en' : $logo = "img/logo-en.png"; break; 
		case 'pl' : $logo = "img/logo.png"; break;
	}
	print("<img id=\"logo\" src=\"".$logo."\" alt=\"".$gameTitle."\"/>");
	print("<h1>".$gameTitle."</h1>");
	print("<h2>".$gameVersion."</h2>");
	?>
	<img id="scrn" src="img/scrn.jpg" alt="screenshot" />
	</div>
	<div id="content">
	<a href="index.php?lang=pl&from=<?php echo $from?>">
		<img src="img/icon_pl.png" alt="Język polski" title="Język polski" border="0">
	</a>
	<a href="index.php?lang=en&from=<?php echo $from?>">
		<img src="img/icon_en.png" alt="English language" title="English language" border="0">
	</a>
	<a href="http://www.indiedb.com/games/warlocks-gauntlet">
	    <?php
			switch ($lang) {
				case 'en' : $title = "See our profile on IndieDB"; break; 
				case 'pl' : $title = "Odwiedź nasz profil na IndieDB"; break;
			}
			print("<img src=\"img/indiedb.png\" alt=\"IndieDB\" title=\"".$title."\" border=\"0\">");
		?>
	</a>
	<a href="http://www.desura.com/games/warlocks-gauntlet">
	    <?php
			switch ($lang) {
				case 'en' : $title = "See WG on Desura"; break; 
				case 'pl' : $title = "WG na Desurze"; break;
			}
			print("<img src=\"http://media.desura.com/images/global/desura.png\" alt=\"Desura\" title=\"".$title."\" border=\"0\">");
		?>
	</a>
