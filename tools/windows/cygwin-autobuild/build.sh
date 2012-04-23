#!/bin/bash
# wczytujemy konfiguracje
. config.sh

echo "Updating working copy..."
svn update "$PROJ_PATH"

echo "Creating directory..."
CURRENT_DIR=$(cygpath -w "$PWD")
REVISION=$(svn info "$PROJ_PATH" | grep Revision: | cut -d" " -f2 | cut -c1-4)
MY_OUTPUT_FOLDER_NAME="$OUTPUT_FOLDER_NAME-"$(date "+%d-%m-%Y")"-r"$REVISION
OUTPUT_DIR="$CURRENT_DIR/$MY_OUTPUT_FOLDER_NAME"
echo "Output dir is $OUTPUT_DIR"

WG_BINARY_NAME="WarlocksGauntlet.exe"

mkdir -p "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/user"

echo "Building..."
cd "$SLN_PATH/$SLN_CONFIG"
rm -f "$WG_BINARY_NAME"
cd "$SLN_PATH"
"$DEVENV_PATH" "$SLN_NAME" /build "$SLN_CONFIG" /out "$CURRENT_DIR/$SLN_CONFIG".log

echo "Exporting..."
cd "$SLN_CONFIG"
cp "$WG_BINARY_NAME" "$OUTPUT_DIR/"
if [ "$?" -ne 0 ]; then echo "build failed, $WG_BINARY_NAME not found"; exit 1; fi 
cp "libsndfile-1.dll" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/msvcr90.dll" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/msvcp90.dll" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/WarlocksGauntlet.exe.manifest" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/WGEditor.exe.manifest" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/Microsoft.VC90.CRT.manifest" "$OUTPUT_DIR/"
cp "$CURRENT_DIR/openal32.dll" "$OUTPUT_DIR/"
#cp "$CURRENT_DIR/crimson-mode.bat" "$OUTPUT_DIR/"
svn export "$PROJ_PATH/data" "$OUTPUT_DIR/data"
svn export "$PROJ_PATH/editor" "$OUTPUT_DIR/editor"
svn export "$PROJ_PATH/data.vfs" "$OUTPUT_DIR/data.vfs"
svn export "$PROJ_PATH/build/wg64.ico" "$OUTPUT_DIR/wg64.ico"
#svn export "$PROJ_PATH/README.txt" "$OUTPUT_DIR/README.txt"

echo "Fetching editor binary..."
cd "$EDITOR_PATH"
svn update
cp "bin/WGEditor.exe" "$OUTPUT_DIR/"

echo "Packaging polish..."
cd "$CURRENT_DIR/$MY_OUTPUT_FOLDER_NAME"
zip --quiet -r "../$MY_OUTPUT_FOLDER_NAME.zip" *

echo "Packaging english..."
cd "$CURRENT_DIR"
sed -e 's@<locale lang="pl"/>@<locale lang="en"/>@' "$MY_OUTPUT_FOLDER_NAME/data/config.xml" >"config.xml.tmp"
cp "config.xml.tmp" "$MY_OUTPUT_FOLDER_NAME/data/config.xml"
rm "config.xml.tmp"
cd "$CURRENT_DIR/$MY_OUTPUT_FOLDER_NAME"
zip --quiet -r "../$MY_OUTPUT_FOLDER_NAME-en.zip" *

echo "Packaging installer..."

rm -rf "$CURRENT_DIR/WG"
cp -r "$OUTPUT_DIR/" "$CURRENT_DIR/WG"
"$NSIS_PATH" "$CURRENT_DIR/WG.nsi"
rm -rf "$CURRENT_DIR/WG"

echo "Done!"