#!/bin/bash

package_name=$1
if [ -n "$package_name" ]
then
    package_name="WarlocksGauntlet-$package_name"
else
    revision=`hg describe`
    DATE=`date +%F_%H-%M-%S`
    package_name="WarlocksGauntlet-"$revision"_"$DATE
fi

rm -rf deb
cp -r tools/deb_image deb
mkdir deb/opt

GAMEDIR=deb/opt/WarlocksGauntlet

cp -rf deploy $GAMEDIR
sed -e "s@<locale lang=\"pl\"/>@<locale lang=\"en\"/>@" "deploy/data/config.xml" > "$GAMEDIR/data/config.xml"

mv deb "$package_name"

dpkg-deb -b "$package_name"

DEBFILE="$package_name".deb
chmod 644 "$DEBFILE"
scp "$DEBFILE" "hell:public_html/all/warsztat/snapshots/linux/en"
#scp "$DEBFILE" "zodiac:public_html/wg/snapshots/linux/en"
rm "$DEBFILE"
rm -rf "$package_name"
