#!/usr/bin/env bash

echo "wasm_sdl2"

DST_PATH="$1"

BUILD_DIR="/home/adam/Repos/GamePunkRun/game_punk/src/pltfm/wasm_sdl2"
OUT_DIR="/home/adam/Repos/GamePunkRun/game_punk/build/wasm_sdl2"

BIN_SRC="$OUT_DIR/punk_run.bin"
HTML_SRC="$OUT_DIR/punk_run.html"
JS_SRC="$OUT_DIR/punk_run.js"
WASM_SRC="$OUT_DIR/punk_run.wasm"

BIN_DST="$DST_PATH/punk_run.bin"
HTML_DST="$DST_PATH/index.html"
JS_DST="$DST_PATH/punk_run.js"
WASM_DST="$DST_PATH/punk_run.wasm"


COPY_DST="$DST_PATH/"

echo -n " create release directory"
mkdir -p $DST_PATH
echo " ok"

echo -n " build executable"
cd $BUILD_DIR
make build
echo " ok"

echo -n " copy release files"
cp -f $BIN_SRC $BIN_DST
cp -f $HTML_SRC $HTML_DST
cp -f $JS_SRC $JS_DST
cp -f $WASM_SRC $WASM_DST
echo " ok"

# chmod +x wasm_sdl2.sh
# source ~/Repos/zRepos/emsdk/emsdk_env.sh