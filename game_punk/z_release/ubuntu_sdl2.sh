#!/usr/bin/env bash

echo "ubuntu_sdl2"

DST_PATH="$1"

BUILD_DIR="/home/adam/Repos/GamePunkRun/game_punk/src/pltfm/ubuntu_sdl2"
OUT_DIR="/home/adam/Repos/GamePunkRun/game_punk/src/pltfm/ubuntu_sdl2/zig-out/bin"

COPY_SRC="$OUT_DIR/*"
COPY_DST="$DST_PATH/"

echo -n " create release directory"
mkdir -p $DST_PATH
echo " ok"

echo -n " build executable"
cd $BUILD_DIR
zig build
echo " ok"

echo -n " copy release files"
cp -f $COPY_SRC $COPY_DST
echo " ok"

# chmod +x ubuntu_sdl2.sh