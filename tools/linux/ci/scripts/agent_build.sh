#!/bin/bash
echo "agent_build" >> log

make -f Makefile.ci > build_stdout.log 2> build_stderr.log
if [ $? -ne 0 ]
then
  exit 1
fi

export LD_LIBRARY_PATH=./tools/linux/ci/libs
./WarlocksGauntlet.bin32 --locale-check >> build_stdout.log 2> build_stderr.log
if [ $? -ne 0 ]
then
  exit 2
fi
./WarlocksGauntlet.bin32 --resource-check >> build_stdout.log 2> build_stderr.log
if [ $? -ne 0 ]
then
  exit 3
fi

exit 0
