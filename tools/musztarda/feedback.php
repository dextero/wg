<?php
include("functions/init.php");
include("functions/feedback.php");

include("header.php");
	
if ($_POST['capu'] !== '6')
{
	switch($lang) {
		case "en" : echo '<h1>Wrong captcha answer! Try Again!</h1>'; break;
	    case "pl" : echo '<h1>Niepoprawny kod! Spróbuj ponownie!</h1>';	break;
	}
}
else
{
	$author = makeSafe($_POST['author']);
	$contact = makeSafe($_POST['contact']);
	$feedback = makeSafe($_POST['feedback']);
	collectFeedback($author, $contact, $feedback, $from, $lang);
		
	switch ($lang) {
		case "en" : echo '<h1>Thanks!</h1> '; break;
		case "pl" : echo '<h1>Dzięki!</h1> '; break;
	}
}

switch ($lang) {
	case "en" : $backTitle = "Go back"; break;
	case "pl" : $backTitle = "Powrót"; break;
}
print("<h3><a href=\"index.php?lang=".$lang."&from=".$from."\">".$backTitle."</a></h3>");

include("footer.php");
?>
