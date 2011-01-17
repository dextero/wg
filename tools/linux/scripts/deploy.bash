#!/bin/bash

function fail()
{
    echo $1
    rm deploy -rf
    exit 1
}

package_name=$1

gamename="WarlocksGauntlet"

binary="$gamename.bin32"
rm $binary
rm deploy -rf

make clean || fail
make release || fail

test -f $binary || fail "$binary not found, exiting"

rm -rf deploy
mkdir deploy
hg archive -I data deploy
hg archive -I user deploy
cp data.vfs deploy
cp tools/linux/launcher/WarlocksGauntlet deploy
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

