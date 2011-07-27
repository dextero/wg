#!/bin/bash
# wczytujemy konfiguracje
. config.sh

echo "Exporting working copy..."
rm -rf "$WORK_DIR"
hg archive -I ../../../src -I ../../../build "$WORK_DIR"

echo "Creating output directory..."
CURRENT_DIR=$(pwd)
REVISION=$(hg describe -l 10000)
DATE=`date +%F_%H-%M-%S`
PACKAGE_NAME="wg_win32_"$REVISION"_"$DATE

OUTPUT_DIR="$CURRENT_DIR/$PACKAGE_NAME"
echo "Output dir is $OUTPUT_DIR"
rm -rf "$OUTPUT_DIR" ; mkdir "$OUTPUT_DIR"

echo "Building..."
WG_BINARY_NAME="WarlocksGauntlet.exe"
mkdir "$SLN_PATH/$SLN_CONFIG"
rm -f "$SLN_PATH/$SLN_CONFIG/$WG_BINARY_NAME"
echo "\"$DEVENV_PATH\"" "$SLN_PATH/$SLN_NAME" /build "$SLN_CONFIG" /out "build.log" > bat.bat
cmd.exe "/C bat.bat" 

echo "Exporting..."
cd "$CURRENT_DIR/$WORK_DIR"
cp "$WG_BINARY_NAME" "$OUTPUT_DIR/"
if [ "$?" -ne 0 ]; then echo "build failed, $WG_BINARY_NAME not found"; exit 1; fi 
cp "$CURRENT_DIR/../../../libsndfile-1.dll" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/msvcr80.dll" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/msvcp80.dll" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/WG-VC80.exe.manifest" "$OUTPUT_DIR/WarlocksGauntlet.exe.manifest"
cp "$CURRENT_DIR/Microsoft.VC80.CRT.manifest" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/openal32.dll" "$OUTPUT_DIR/"
cd "$CURRENT_DIR"
hg archive -I ../../../data "$OUTPUT_DIR"
hg archive -I ../../../data.vfs "$OUTPUT_DIR"
hg archive -I ../../../build/wg64.ico "$OUTPUT_DIR/wg64.ico"

echo "Zipping package..."
cd "$CURRENT_DIR"
PACKAGE_ZIP="$PACKAGE_NAME.zip"
zip -q -r "$PACKAGE_ZIP" "$PACKAGE_NAME"

echo "Uploading..."
scp "$PACKAGE_ZIP" toxic@hell.org.pl:public_html/all/warsztat/snapshots/win32/en

echo "Done!"