<?php

function getRemoteFile2($url) {
   if (!function_exists('curl_init')) exit("curl_init doesn't exist");
   // initialize a new curl resource
   $ch = curl_init(); 

   // set the url to fetch
   curl_setopt($ch, CURLOPT_URL, 'http://www.google.com'); 

   // don't give me the headers just the content
   curl_setopt($ch, CURLOPT_HEADER, 0); 

   // return the value instead of printing the response to browser
   curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 

   // use a user agent to mimic a browser
   curl_setopt($ch, CURLOPT_USERAGENT, 'Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.5) Gecko/20041107 Firefox/1.0'); 

   $content = curl_exec($ch); 

   // remember to always close the session and free all resources 
   curl_close($ch);
   
   return $content;
}

function getRemoteFile($url) {
   // get the host name and url path
   $parsedUrl = parse_url($url);
   $host = $parsedUrl['host'];
   if (isset($parsedUrl['path'])) {
      $path = $parsedUrl['path'];
   } else {
      // the url is pointing to the host like http://www.mysite.com
      $path = '/';
   }

   if (isset($parsedUrl['query'])) {
      $path .= '?' . $parsedUrl['query'];
   } 

   if (isset($parsedUrl['port'])) {
      $port = $parsedUrl['port'];
   } else {
      // most sites use port 80
      $port = '80';
   }

   $timeout = 10;
   $response = ''; 

   // connect to the remote server 
   $fp = @fsockopen($host, $port, $errno, $errstr, $timeout);

   if( !$fp ) { 
      echo "Cannot retrieve ".$url;
   } else {
      // send the necessary headers to get the file 
      fputs($fp, "GET $path HTTP/1.0\r\n" .
                 "Host: $host\r\n" .
                 "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.3) Gecko/20060426 Firefox/1.5.0.3\r\n" .
                 "Accept: */*\r\n" .
                 "Accept-Language: en-us,en;q=0.5\r\n" .
                 "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n" .
                 "Keep-Alive: 300\r\n" .
                 "Connection: keep-alive\r\n" .
                 "Referer: http://$host\r\n\r\n");

      // retrieve the response from the remote server 
      while ( $line = fread( $fp, 4096 ) ) { 
         $response .= $line;
      } 

      fclose( $fp );

      // strip the headers
      $pos      = strpos($response, "\r\n\r\n");
      $response = substr($response, $pos + 4);
   }

   // return the file content 
   return $response;	
	
}
?>