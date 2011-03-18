#!/bin/bash

function fail()
{
    echo $1
    rm deploy -rf
    exit 1
}

package_name=$1

gamename="WarlocksGauntlet"

binary32="$gamename.bin32"
binary64="$gamename.bin64"
rm $binary32
rm $binary64
rm deploy -rf

make clean || fail
make release || fail

test -f $binary32 || fail "$binary32 not found, exiting"


rm -rf deploy
mkdir deploy
hg archive -I data deploy
hg archive -I user deploy
cp data.vfs deploy
cp tools/linux/launcher/WarlocksGauntlet deploy
mkdir deploy/libs32
cp libs32/libsfml-graphics.so.1.6 deploy/libs32
cp libs32/libsfml-window.so.1.6 deploy/libs32
cp libs32/libsfml-system.so.1.6 deploy/libs32
cp libs32/libsfml-audio.so.1.6 deploy/libs32
cp libs32/libopenal.so.1 deploy/libs32
cp libs32/libsndfile.so.1 deploy/libs32
mkdir deploy/bin
cp $binary32 deploy/bin

g++ tools/linux/check_fullscreen/main.cpp -o deploy/bin/check_fullscreen.bin32 -static-libgcc -O2 /usr/lib/gcc/i486-linux-gnu/4.4.1/libstdc++.a -I"build/includes/SFML-1.6/include" -L"./libs32" -lsfml-graphics -lsfml-window -lsfml-system || fail
cp tools/linux/check_fullscreen/check_fullscreen deploy/bin

#bit64 version:

make clean || fail
make -f Makefile.x86_64 release || fail
test -f $binary64 || fail "$binary64 not found, exiting"
g++ -m64 tools/linux/check_fullscreen/main.cpp -o deploy/bin/check_fullscreen.bin64 -static-libgcc -O2 -I"build/includes/SFML-1.6/include" -L"./libs64" -lsfml-graphics -lsfml-window -lsfml-system || fail

mkdir deploy/libs64
cp libs64/libsfml-graphics.so.1.6 deploy/libs64
cp libs64/libsfml-window.so.1.6 deploy/libs64
cp libs64/libsfml-system.so.1.6 deploy/libs64
cp libs64/libsfml-audio.so.1.6 deploy/libs64
cp libs64/libopenal.so.1 deploy/libs64
cp libs64/libsndfile.so.1 deploy/libs64
cp $binary64 deploy/bin
