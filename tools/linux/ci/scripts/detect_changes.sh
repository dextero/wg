#!/bin/bash
echo "detect_changes" >> log
FILESTAMP="agent_last_revision"
if [ -f $FILESTAMP ]
then
#donothing
echo "FILESTAMP=$FILESTAMP" >> log
else
  rm -rf $FILESTAMP
  echo "0" > $FILESTAMP
fi
declare last=`cat $FILESTAMP`
echo "last=$last" >> log
# warto by rozwazyc przewalenie sciagania repo do osobnego
# pod procesu, co by brak miejsca na dysku byl wylapywany
# jako jednokrotny FAIL...
hg pull http://hg.assembla.com/gdpl -r default >> log
echo "after hg pull" >> log
hg up >> log
echo "after hg up" >> log
declare current=`hg id | cut -d" " -f1`
echo "current=$current" >> log
echo $current > $FILESTAMP
if [ "$current" = "$last" ]
then
  echo "exit 1" >> log
  exit 1
else
  echo "exit 0" >> log
  exit 0
fi
