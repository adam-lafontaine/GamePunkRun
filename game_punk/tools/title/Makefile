GPP := g++-11 -std=c++20 -mavx -mavx2 -mfma
#GPP += -Wall -Wextra

#GPP += -DNDEBUG -O3

GPP += -DIMAGE_READ
GPP += -DIMAGE_WRITE
#GPP += -DIMAGE_RESIZE

SDL := -lSDL3

ALL_LFLAGS := $(SDL)


EXE := gen_icon

ROOT := ..
APP := $(ROOT)/title

FILES := $(APP)/out_files

BUILD := $(FILES)/build

OUT := $(BUILD)/$(EXE)

UTILS := $(ROOT)/utils

SRC := $(APP)/title_main.cpp

DEP := $(SRC)
DEP += $(APP)/title.hpp
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