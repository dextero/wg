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

rm -rf debian
mkdir debian
mkdir debian/opt

rm -rf deploy
mkdir deploy
hg archive -I data deploy
hg archive -I user deploy
cp data.vfs deploy
mkdir deploy/libs32
cp libs32/libsfml-graphics.so.1.4 deploy/libs32
cp libs32/libsfml-window.so.1.4 deploy/libs32
cp libs32/libsfml-system.so.1.4 deploy/libs32
cp libs32/libsfml-audio.so.1.4 deploy/libs32
cp libs32/libopenal.so.1 deploy/libs32
cp libs32/libsndfile.so.1 deploy/libs32
mkdir deploy/bin
cp "WarlocksGauntlet.bin32" deploy/bin

g++ tools/linux/check_fullscreen/main.cpp -o deploy/bin/check_fullscreen -static-libgcc -O2 /usr/lib/gcc/i486-linux-gnu/4.4.1/libstdc++.a -I"build/includes/SFML-1.4/include" -L"./libs32" -lsfml-graphics -lsfml-window -lsfml-system || exit
g++ tools/linux/launcher/main.cpp -o deploy/launcher -static-libgcc -O2 /usr/lib/gcc/i486-linux-gnu/4.4.1/libstdc++.a || exit
sed -e "s@<locale lang=\"pl\"/>@<locale lang=\"en\"/>@" "deploy/data/config.xml" > "deploy/data/config-en.xml" && mv "deploy/data/config-en.xml" "deploy/data/config.xml"
mv "deploy/launcher" "deploy/WarlocksGauntlet"

mv deploy debian/opt/WarlocksGauntlet

mkdir debian/DEBIAN
cp debian.control debian/DEBIAN/control

#todo: ikonki itepe, do debian/usr, ikonka na desktopie itd -> patrz WorldOfGoo.deb, 
#aha, i skrot w /usr/local/costam, bo inaczej komenda "WarlocksGauntlet" nie jest rozpoznawana


mv "debian" "$package_name"

dpkg-deb -b "$package_name"

DEBFILE="$package_name".deb
chmod 644 "$DEBFILE"
scp "$DEBFILE" "hell:public_html/all/warsztat/snapshots/linux/en"
rm "$DEBFILE"
rm -rf "$package_name"
