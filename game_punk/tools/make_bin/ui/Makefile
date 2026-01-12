GPP := g++-11 -std=c++20 -mavx -mavx2 -mfma
#GPP += -Wall -Wextra

#GPP += -DNDEBUG -O3

GPP += -DIMAGE_READ
GPP += -DIMAGE_WRITE
#GPP += -DIMAGE_RESIZE

SDL := -lSDL3

ALL_LFLAGS := $(SDL)


EXE := gen_ui

ROOT := ../..
APP_ROOT := $(ROOT)/make_bin
APP := $(APP_ROOT)/ui

FILES := $(APP)/out_files

BUILD := $(FILES)/build

OUT := $(BUILD)/$(EXE)

UTILS := $(ROOT)/utils

SRC := $(APP)/ui_main.cpp

DEP := $(SRC)
DEP += $(APP)/ui.hpp
DEP += $(UTILS)/tools_include.hpp
DEP += $(UTILS)/main_sdl3_o.cpp

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
	rm -rfv $(FILES)/*