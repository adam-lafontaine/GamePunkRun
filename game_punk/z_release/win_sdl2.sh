#!/usr/bin/env bash

echo "win_sdl2"

DST_PATH="$1"

DST_PATH="$1"

BUILD_DIR="/home/adam/Repos/GamePunkRun/game_punk/src/pltfm/win_sdl2"
OUT_DIR="/home/adam/Repos/GamePunkRun/game_punk/src/pltfm/win_sdl2/zig-out/bin"

COPY_SRC="$OUT_DIR/*"
COPY_DST="$DST_PATH/"

DST_PDB="$DST_PATH/punk_run.pdb"

echo -n " create release directory"
mkdir -p $DST_PATH
echo " ok"

echo -n " build executable"
cd $BUILD_DIR
zig build
echo " ok"

echo -n " copy release files"
cp -f $COPY_SRC $COPY_DST
rm $DST_PDB
echo " ok"

# chmod +x win_sdl2.sh