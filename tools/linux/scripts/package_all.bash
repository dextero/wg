#!/bin/bash

if [ -n "$1" ]
then
    package_version="$1"
    scm_revision=$build_name
else
    DATE=`date +%F_%H-%M-%S`
    scm_revision=`hg describe -l 10000`
    package_version="$scm_revision"_"$DATE"
fi

./tools/linux/scripts/deploy.bash $scm_revision || exit 1
./tools/linux/scripts/package_tgz.bash $package_version $scm_revision || exit 1
./tools/linux/scripts/package_deb.bash $package_version $scm_revision || exit 1
rm -rf deploy
