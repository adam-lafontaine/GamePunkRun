#!/usr/bin/env bash

RELEASE_ROOT="/home/adam/Repos/ReleaseFiles/PunkRun"

VERSION="${1:-vXXX}"


DST_ROOT="$RELEASE_ROOT/$VERSION"

echo "Release All"
echo "$(date)"

./ubuntu_sdl2.sh "$DST_ROOT/Punk_Ubuntu"
./win_sdl2.sh "$DST_ROOT/Punk_Windows"
./wasm_sdl2.sh "$DST_ROOT/Punk_Wasm"

# chmod +x all.sh
# source ~/Repos/zRepos/emsdk/emsdk_env.sh