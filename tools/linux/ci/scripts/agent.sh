#!/bin/bash
rm log
echo jestem agent!
./detect_changes.sh
declare -i has_repo_changed=$?
if [ $has_repo_changed -eq 0 ]
then
  ./agent_build.sh
  ./serwer_update_status.sh $?
fi
