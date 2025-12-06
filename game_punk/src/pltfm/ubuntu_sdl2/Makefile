GPP := g++-11 -std=c++20
GPP += -mavx -mavx2 -mfma
#GPP += -Wall -Wextra

GPP += -DGAME_PUNK_RELEASE
GPP += -DAPP_ROTATE_90
#GPP += -DAPP_FULLSCREEN

#GPP += -DAPP_ASSERT_LOG
GPP += -DNDEBUG -O3

GPP += -DIMAGE_READ
#GPP += -DIMAGE_WRITE
#GPP += -DIMAGE_RESIZE
#GPP += -DALLOC_COUNT

EXE := punk_run

NO_FLAGS := 
SDL := -lSDL2
SDL_AUDIO := -lSDL2_mixer
THREAD := -lpthread -ltbb

ALL_LFLAGS := $(SDL) $(SDL_AUDIO) $(THREAD)

ROOT := ../../..

BUILD := $(ROOT)/build/ubuntu_sdl2

RES := $(ROOT)/res

BIN_DATA := $(RES)/xbin/punk_run.bin

OUT := $(BUILD)/$(EXE)

SRC := ubuntu_sdl2_punk_main.cpp


#****************

build:
	$(GPP) -o $(OUT) $(SRC) $(ALL_LFLAGS)
	cp $(BIN_DATA) $(BUILD)


clean:
	rm -rfv $(BUILD)/*


setup:
	mkdir -p $(BUILD)


delete:
	rm -rfv $(BUILD)