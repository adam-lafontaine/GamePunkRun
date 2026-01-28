
# ~/Repos/zRepos/emsdk
# git pull
# ./emsdk install latest
# ./emsdk activate latest

# Activate PATH and other environment variables in the current terminal
# source ~/Repos/zRepos/emsdk/emsdk_env.sh

# python3 -m http.server 8080


EPP := em++

EPPFLAGS := -std=c++23
#EPPFLAGS += -mavx -msimd128
EPPFLAGS += -DMATH_NO_SIMD
EPPFLAGS += -DGAME_PUNK_RELEASE
EPPFLAGS += -DGAME_PUNK_WASM
EPPFLAGS += -DAPP_ROTATE_90
EPPFLAGS += -DIMAGE_READ

EPPFLAGS += -DNDEBUG -O3
#EPPFLAGS += -DAPP_ASSERT_LOG

EPPFLAGS += -sASSERTIONS=1
EPPFLAGS += -sUSE_SDL=2
#EPPFLAGS += -sUSE_SDL_MIXER=2
#EPPFLAGS += -sUSE_OGG=1
EPPFLAGS += -sFETCH=1
EPPFLAGS += -sALLOW_MEMORY_GROWTH=1

CMSFLAGS := -DCMS_BIN_DATA

EXE := punk_run

HTMLFLAGS := 
HTMLFLAGS += --shell-file out_template.html

LDFLAGS :=

ROOT := ../../..

BUILD := $(ROOT)/build/wasm_sdl2

OUT := $(BUILD)/$(EXE).html

OUT_WASM := $(BUILD)/$(EXE).wasm
OUT_JS   := $(BUILD)/$(EXE).js

CMS := $(ROOT)/../../CMS/sm/wasm

RES := $(ROOT)/res

BIN_DATA := $(RES)/xbin/punk_run.bin

SRC := wasm_punk_main.cpp


#**********************

build:
	$(EPP) $(EPPFLAGS) $(HTMLFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)
	cp $(BIN_DATA) $(BUILD)
	

cms:
	$(EPP) $(EPPFLAGS) $(CMSFLAGS) $(HTMLFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)
	cp -u -v $(OUT_WASM) $(CMS)
	cp -u -v $(OUT_JS) $(CMS)
	cp -u -v $(BIN_DATA) $(CMS)


clean:
	rm -rfv $(BUILD)/*


setup:
	mkdir -p $(BUILD)


delete:
	rm -rfv $(BUILD)