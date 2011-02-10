<?php
include("functions/init.php");
include("functions/logger.php");
include("functions/remote.php");

$platform = $_GET["platform"];
if (empty($platform)) exit("error, empty platform");

$newestLookup = array(
	"win_pl" => "http://students.mimuw.edu.pl/~sk236086/WG/paczki/newest.php",
	"win_en" => "http://students.mimuw.edu.pl/~sk236086/WG/paczki/newest.php",
	"lin_pl" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/linux/pl/latest.php",
	"lin_en" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/linux/en/latest.php",
	"mac_pl" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/mac/latest.php",
	"mac_en" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/mac/latest.php"
);	

$key = $platform."_".$lang;
$url = trim(getRemoteFile($newestLookup[$key]), " \t\n");
if (empty($url)) exit("error, url is empty");

$newestStorage = array(
	"win_pl" => "http://students.mimuw.edu.pl/~sk236086/WG/paczki/",
	"win_en" => "http://students.mimuw.edu.pl/~sk236086/WG/paczki/",
	"lin_pl" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/linux/pl/",
	"lin_en" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/linux/en/",
	"mac_pl" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/mac/",
	"mac_en" => "http://hell.org.pl/~toxic/all/warsztat/snapshots/mac/"
);

$url = $newestStorage[$key].$url;

header("location: ".$url);
logMessage("snapshot (".$url.") was downloaded from ".$from);	
?>
