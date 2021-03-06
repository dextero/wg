<?php
include("functions/init.php");

include("header.php");
?>
	<img src="img/s2.jpg" class="pic" />
<?php
	switch ($lang) {
		case 'en' :
			print("<h1>Introduction</h1>"
					."<p>Warlock's Gauntlet is an Open-Source community project,"
					." created by users from gamedev.pl - a polish game development"
					." site known as Warsztat (the Workshop).</p>"
					."<p> On this page you can download our latest demo - a stable and"
					." tested version that should provide at least two hours of"
					." monster-whacking fun. However, if you like a bit of risk,"
					." there are unstable versions (nightly builds) available"
					." for download at <a href=\"#snapshots\">the bottom</a> of the page.</p>");
			break;
		case 'pl' :
			print("<h1>Informacje</h1>"
					."<p>Warsztat Game to w wielkim skrócie projekt społecznościowy"
					." Open Source tworzony w zamyśle przez osoby związane z portalem gamedev.pl.</p>"
					."<p>Demo, do którego linki znajdziesz poniżej, zostało przygotowane tak,"
					." aby zapewnić około 2 godzin gry i jest uznane za kandydata na wersję \"stabilną\"."
					." Jeśli jednak lubisz trochę ryzyka, na samym <a href=\"#snapshots\">końcu</a>"
					." znajdziesz linki do paczek deweloperskich.</p>");
			break;
		}
	?>
	
	<?php
	include("functions/downloads.php");
	print("<h1>Download</h1>");
	print("<img src=\"img/s1.jpg\" class=\"pic\" />");
	foreach(array("win", "lin", "mac") as $platform) {
		print("<p>");
		switch($platform) {
			case "win" : print("Windows XP/Vista/7:"); break;
			case "lin" : 
				print("Linux");
				switch($lang) {
					case "en" : print(" (tested on Ubuntu 10.4)"); break;
					case "pl" : print(" (testowano na Ubuntu 10.4)"); break;
				}
				print(":");
				break;
			case "mac" : print("MacOS:"); break;
		}
		print("</p><ul>");
		foreach (getDownloadsMatching($lang, $platform, null) as $package) {
			print("<li><a href=\"download.php?lang=".$lang."&platform=".$platform."&title=".$package->title."&from=".$from."\">".$package->title."</a></li>");
		}
		print("</ul>");
	}

	switch($lang) {
		case 'en' :
			print("<h1>Contact</h1>"
					."<p>Do you like it? Maybe not? What are the good and bad points of this game?"
					." Share your opinion and help our team improve the game, kill bugs, create"
					." better, faster, stronger..erm..nvm ;)</p>"
					."<p>Just use the form below to give as feedback (and for eternal glory! Howgh!)</p>");
			$fieldNick = "Your nick:";
			$fieldContact = "Contact (mail, aim etc, optionally):";
			$fieldFeedback = "Your opinion:";
			break;		
		case 'pl' :
			print("<h1>Kontakt</h1>"
					."<p>Spodobało się? Może nie? Co w tej grze jest fajnego a co nie?"
					." Podziel się swoją opinią i pomóż w ten sposób naszemu zespołowi"
					." sprawić by gra była lepsza, pozbyć się błędów, stworzyć coś naprawdę fajnego ;)</p>"
					." <p>Wystarczy, że użyjesz poniższego formularza aby przekazać nam swoje uwagi"
					." (i zyskać wieczną chwałę! Howgh!).</p>");
			$fieldNick = "Twój nick:";
			$fieldContact = "Kontakt (mail, gg etc, opcjonalnie):";
			$fieldFeedback = "Twoja opinia:";
			break;
	}
	?>
	<form method="post" <?php print("action=\"feedback.php?lang=".$lang."&from=".$from."\">"); ?>
		<fieldset style="width:600px">
		<legend>Feedback</legend>
		<table>
		<tr><td width="220"><?php print($fieldNick); ?>    </td><td width="330"> <input type="text" name="author" maxlength="15" />  </td></tr>
		<tr><td width="220"><?php print($fieldContact); ?> </td><td width="330"> <input type="text" name="contact" maxlength="32" /> </td></tr>
		<tr><td width="220">Captcha (2+2*2)                </td><td width="330"> <input type="text" name="capu" maxlength="1"/>      </td></tr>
		<tr><td colspan="2"><?php print($fieldFeedback); ?> <br/>
		<textarea style="width:550px" name="feedback" cols="100" rows="8"></textarea> </td></tr>
		</table>
		    <?php
				switch($lang) {
					case "en" : $feedbackButton = "Send feedback"; break;
					case "pl" : $feedbackButton = "Wyślij opinię"; break;
				}
			?>
		<input type="submit" value="<?php echo $feedbackButton ?>"/>
		</fieldset>
	</form>
	<a name="snapshots"></a>
	<?php
	switch ($lang) {
		case 'en' : 
			print("<h1>Developer Version (unstable)</h1>"
					."<p>So, you like a bit of risk? Below are the newest packages built from the repository."
					." They contain new features, but are untested and potentially unstable (please notice"
					." that they are in polish language for now only):</p>");
			break;
		case 'pl' :
			print("<h1>Wersja deweloperska (niestabilna)</h1>"
					."<p>Hmm lubisz ryzyko? :) Poniżej najnowsze paczki które mogą mieć"
					." w porównaniu z wersją stabilną dodatkową funkcjonalnośc i"
					." bajery kosztem stabilności:</p>");
			break;
	}
	?>
	<ul>
	<?php
	foreach(array("win" => "Windows", "lin" => Linux, "mac" => "MacOS") as $platform => $title) {
		print("<li><a href=\"snapshot.php?lang=".$lang."&platform=".$platform."&from=".$from."\">".$title."</a></li>");
	}
	?>
	</ul>

<?php include "footer.php" ?>