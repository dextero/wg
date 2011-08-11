#!/bin/bash

package_version=$1
scm_revision=$2

gamename="WarlocksGauntlet"

if [ -z "$scm_revision" ]
then
    scm_revision=`hg describe -l 10000`
fi

if [ -n "$package_version" ]
then
    package_name="wg_linux_$package_version"
else
    DATE=`date +%F_%H-%M-%S`
    package_name="wg_linux_"$scm_revision"_"$DATE
fi

for lang in "pl" "en"
do
    DIR="$package_name"_"$lang"
    cp -rf deploy "$DIR"
    sed -e "s@<locale lang=\"pl\"/>@<locale lang=\"$lang\"/>@" "deploy/data/config.xml" > "$DIR/data/config.xml"
    if [ $lang = "pl" ]
    then
        mv "$DIR/WarlocksGauntlet" "$DIR/WarsztatGame"
    fi
    if [ $lang = "en" ]
    then
        mv "$DIR/WarlocksGauntlet" "$DIR/WarlocksGauntlet"
    fi
    TARFILE=$DIR".tar.gz"
    tar -czf "$TARFILE" "$DIR"
    chmod 644 "$TARFILE"
    scp "$TARFILE" "hell:public_html/all/warsztat/snapshots/linux/$lang"
#    scp "$TARFILE" "zodiac:public_html/wg/snapshots/linux/$lang"
    rm "$TARFILE"
    rm -rf "$DIR"
done
