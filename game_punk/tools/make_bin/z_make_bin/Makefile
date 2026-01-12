GPP := g++-11 -std=c++20 -mavx -mavx2 -mfma
#GPP += -Wall -Wextra

#GPP += -DNDEBUG -O3

GPP += -DIMAGE_READ
GPP += -DIMAGE_WRITE
GPP += -DIMAGE_RESIZE

EXE := make_bin

SDL := -lSDL3

ALL_LFLAGS := $(SDL)

ROOT := ../..
APP_ROOT := $(ROOT)/make_bin
APP := $(APP_ROOT)/z_make_bin
BG := $(APP_ROOT)/background
SKY := $(APP_ROOT)/sky
SPRITE := $(APP_ROOT)/sprite
TILE := $(APP_ROOT)/tile
UI := $(APP_ROOT)/ui

FILES := $(APP)/out_files

BUILD := $(FILES)/build

OUT := $(BUILD)/$(EXE)

UTILS := $(ROOT)/utils

SRC := $(APP)/make_bin_main.cpp

DEP := $(SRC)
DEP += $(APP)/make_bin.hpp
DEP += $(APP)/bin_def.hpp
DEP += $(APP)/bin_table_str.hpp
DEP += $(UTILS)/tools_include.hpp
DEP += $(UTILS)/main_sdl3_o.cpp
DEP += $(SKY)/sky.hpp
DEP += $(BG)/background.hpp
DEP += $(SPRITE)/sprite.hpp
DEP += $(TILE)/tile.hpp
DEP += $(UI)/ui.hpp


#****************

build: $(DEP)
	$(GPP) -o $(OUT) $(SRC) $(ALL_LFLAGS)


run: build
	$(OUT)


clean:
	rm -rfv $(BUILD)/*


setup:
	mkdir -p $(FILES)
	mkdir -p $(BUILD)


delete:
	rm -rfv $(FILES)