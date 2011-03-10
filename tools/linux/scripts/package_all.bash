#!/bin/bash

if [ -n "$1" ]
then
    package_name="$1"
else
    revision=`hg describe`
    DATE=`date +%F_%H-%M-%S`
    package_name="$revision"_"$DATE"
fi

./tools/linux/scripts/deploy.bash || exit 1
./tools/linux/scripts/package_tgz.bash $package_name || exit 1
./tools/linux/scripts/package_deb.bash $package_name || exit 1
rm -rf deploy
