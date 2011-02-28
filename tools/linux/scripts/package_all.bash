#!/bin/bash

if [ -n "$package_name" ]
then
    package_name="$package_name"
else
    revision=`hg describe`
    DATE=`date +%F_%H-%M-%S`
    package_name="$revision"_"$DATE"
fi

./tools/linux/scripts/deploy.bash
./tools/linux/scripts/package_tgz.bash $package_name
./tools/linux/scripts/package_deb.bash $package_name
rm -rf deploy
