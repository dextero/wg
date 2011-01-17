#!/bin/bash

package_name=$1

gamename="WarlocksGauntlet"

if [ -n "$package_name" ]
then
    package_name="wg_linux_$package_name"
else
    revision=`hg describe`
    DATE=`date +%F_%H-%M-%S`
    package_name="wg_linux_"$revision"_"$DATE
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
    rm "$TARFILE"
    rm -rf "$DIR"
done
