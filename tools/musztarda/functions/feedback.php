<?php

function sanityCheck($string, $type, $length){

  // assign the type
  $type = 'is_'.$type;

  if(!$type($string))
    {
    return FALSE;
    }
  // now we see if there is anything in the string
  else if(empty($string))
    {
    return FALSE;
    }
  // then we check how long the string is
  else if(strlen($string) > $length)
    {
    return FALSE;
    }
    // if all is well, we return TRUE
    return TRUE;
}

function sendMail($content, $sender)
{
    $to = 'gdpl@tickets.assembla.com';
#    $to = 'ps209499@zodiac.mimuw.edu.pl';
	$subject = 'Od '.$sender;
	$headers = 'From: nobody@srv1.whshost.com' . "\r\n" .
    'Reply-To: nobody@srv1.whshost.com' . "\r\n" .
    'X-Mailer: PHP/' . phpversion();
    
    $message = "Milestone: Feedback\nPriority: 3\nDescription: \n".$content.".";
    
    mail($to, $subject, $message, $headers);
}

function makeSafe($var)
{
    return htmlspecialchars(strip_tags($var));
}

function collectFeedback($author, $contact, $feedback, $from, $lang) {

	$content = date("F j, Y, g:ia\n");
	$content .= "Autor: ".$author.", ".$_SERVER['REMOTE_ADDR']." \n";
	$content .= "Kontakt: ".$contact."\n";
	$content .= "Lang: ".$lang.", from: ".$from."\n";
	$content .= "Opinia: \n    ".str_replace("\n", "\n    ", $feedback)."\n\n";

	$file = fopen('feedback.txt', 'a');
	fwrite($file, $content);
	fclose($file);
		
	sendMail($content, $author);
}
	
?>