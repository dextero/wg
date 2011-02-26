#!/bin/bash

TRIGGER_PAGE=http://students.mimuw.edu.pl/~ps209499/wg/serwer_update_status.php
PHP_PARAMS="ticketNo=980"
WGET_PARAMS="-q -o /dev/null -O /dev/null"

echo jestem serwer_update_status!

declare revision=`hg id | cut -d" " -f1`

#log_dir="hell:public_html/WG-log/2"
#copy=scp

log_dir="$HOME/public_html/wg/logs"
copy=cp

$copy build_stdout.log "$log_dir/r$revision-build_stdout.log"
$copy build_stderr.log "$log_dir/r$revision-build_stderr.log"



if [ $1 -eq 0 ]
# dziala, build successful
then
  URL="$TRIGGER_PAGE?$PHP_PARAMS&rev=$revision&status=OK"
  wget "$URL" $WGET_PARAMS
  echo `date` "$URL" >> serwer_update_status.log
fi

if [ $1 -eq 1 ]
# niedziala, broken
then
  URL="$TRIGGER_PAGE?$PHP_PARAMS&rev=$revision&status=FAILED"
  wget "$URL" $WGET_PARAMS
  echo `date` "$URL" >> serwer_update_status.log
fi

if [ $1 -eq 2 ]
# --check-locale zfailowal
then
  URL="$TRIGGER_PAGE?$PHP_PARAMS&rev=$revision&status=LOCALE_FAILED"
  wget "$URL" $WGET_PARAMS
  echo `date` "$URL" >> serwer_update_status.log
fi

if [ $1 -eq 3 ]
# --check-resource zfailowal
then
  URL="$TRIGGER_PAGE?$PHP_PARAMS&rev=$revision&status=RESOURCE_FAILED"
  wget "$URL" $WGET_PARAMS
  echo `date` "$URL" >> serwer_update_status.log
fi
