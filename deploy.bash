#!/bin/bash

function fail()
{
    rm deploy -rf
    exit 1
}


#svn cleanup
#svn up

package_name=$1

gamename="WarlocksGauntlet"

binary="$gamename.bin32"
rm $binary
rm deploy -rf

make clean || fail
make release || fail

test -f $binary || echo "$binary not found, exiting" || fail
echo "found binary"

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
cp $binary deploy/bin

g++ tools/linux/check_fullscreen/main.cpp -o deploy/bin/check_fullscreen -static-libgcc -O2 /usr/lib/gcc/i486-linux-gnu/4.4.1/libstdc++.a -I"build/includes/SFML-1.4/include" -L"./libs32" -lsfml-graphics -lsfml-window -lsfml-system || fail
g++ tools/linux/launcher/main.cpp -o deploy/launcher -static-libgcc -O2 /usr/lib/gcc/i486-linux-gnu/4.4.1/libstdc++.a || fail

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
        mv "$DIR/launcher" "$DIR/WarsztatGame"
    fi
    if [ $lang = "en" ]
    then
        mv "$DIR/launcher" "$DIR/WarlocksGauntlet"
    fi
    TARFILE=$DIR".tar.gz"
    tar -czf "$TARFILE" "$DIR"
    chmod 644 "$TARFILE"
    scp "$TARFILE" "hell:public_html/all/warsztat/snapshots/linux/$lang"
    rm "$TARFILE"
    rm -rf "$DIR"
done

rm deploy -rf
