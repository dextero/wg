#!/bin/bash

package_name=$1
if [ -n "$package_name" ]
then
    revision=$package_name
    package_name="WarlocksGauntlet-$package_name"
else
    revision=`hg describe -l 10000`
    DATE=`date +%F_%H-%M-%S`
    package_name="WarlocksGauntlet-"$revision"_"$DATE
fi

rm -rf deb
cp -r tools/deb_image deb
mkdir deb/opt

GAMEDIR=deb/opt/WarlocksGauntlet

cp -rf deploy $GAMEDIR
SIZE=`du $GAMEDIR -s | cut -f1`
sed -e "s/"'${version}'"/$revision/g" -e "s/"'${size}'"/$SIZE/g" tools/deb_image/DEBIAN/control > deb/DEBIAN/control

mv deb "$package_name"

dpkg-deb -b "$package_name"

DEBFILE="$package_name".deb
chmod 644 "$DEBFILE"
scp "$DEBFILE" "hell:public_html/all/warsztat/snapshots/linux/en"
#scp "$DEBFILE" "zodiac:public_html/wg/snapshots/linux/en"
rm "$DEBFILE"
rm -rf "$package_name"
