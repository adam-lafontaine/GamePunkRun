GPP := g++-11 -std=c++20 -mavx -mavx2 -mfma
#GPP += -Wall -Wextra

#GPP += -DNDEBUG -O3

GPP += -DIMAGE_READ
GPP += -DIMAGE_WRITE

SDL := -lSDL3

ALL_LFLAGS := $(SDL)


EXE := bg_to_mask

ROOT := ..
APP := $(ROOT)/background

FILES := $(APP)/out_files

BUILD := $(FILES)/build

OUT := $(BUILD)/$(EXE)

UTILS := $(ROOT)/utils

SRC := $(APP)/background_main.cpp

DEP := $(SRC)
DEP += $(APP)/background.hpp
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