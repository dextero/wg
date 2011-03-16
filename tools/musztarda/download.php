<?php
include("functions/init.php");
include("functions/logger.php");

$platform = $_GET["platform"];
$title = $_GET["title"];

if (empty($platform)) exit("error, empty platform");
if (empty($title)) exit("error, empty title");

include("functions/downloads.php");
$package = getDownloadsMatching($lang, $platform, $title);
if (empty($package)) exit("error, empty package");

$package = $package[0];

header("location: ".$package->url);

logMessage($gameVersion." version (".$package->url.") was downloaded from ".$from);
?>
