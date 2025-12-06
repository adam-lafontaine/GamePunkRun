GPP := g++ -std=c++20
GPP += -mavx -mavx2 -mfma
GPP += -mwindows
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

EXE := punk_run.exe

SDL := C:\SDL2\SDL2-2.30.5\x86_64-w64-mingw32
SDL_MIXER := C:\SDL2\SDL2_mixer-2.8.0\x86_64-w64-mingw32

SDL_DLL := $(SDL)/bin/SDL2.dll
SDL_MIXER_DLL := $(SDL_MIXER)/bin/SDL2_mixer.dll

ISDL := -I$(SDL)\include -I$(SDL)\include\SDL2
ISDL_MIXER=-I$(SDL_MIXER)\include

LDSDL := -L$(SDL)\lib
LDSDL_MIXER := -L$(SDL_MIXER)\lib

IFLAGS := $(ISDL) $(ISDL_MIXER)
LDFLAGS := $(LDSDL) $(LDSDL_MIXER) -lSDL2 -lSDL2_mixer

ROOT := ../../..

BUILD := $(ROOT)/build/win_sdl2
R_BUILD := $(BUILD)/release
O_BUILD := $(BUILD)/o

RES := $(ROOT)/res

BIN_DATA := $(RES)/xbin/punk_run.bin

OUT := $(R_BUILD)/$(EXE)

SRC := win_sdl2_punk_main.cpp

#************

program_exe := $(OUT)


#*** main cpp ***

main_c := $(SRC)
main_o := $(O_BUILD)/main_o

obj := $(main_o)

main_dep := 

#***************


#*** icon ******

icon_rc := punk.rc
icon_o := $(O_BUILD)/icon.o

obj += $(icon_o)

icon_dep := icon_punk_64.ico

#****************


$(main_o): $(main_c) $(main_dep)
	@echo "  --- main ---"
	$(GPP) $(CPPFLAGS) $(IFLAGS) -o $@ -c $< $(LDFLAGS)


$(icon_o): $(icon_rc) $(icon_dep)
	@echo "  --- icon ---"
	@windres -i $< -o $@


$(program_exe): $(obj)
	@echo "  --- link ---"
	$(GPP) $(CPPFLAGS) -o $@ $+ $(LDFLAGS)
	cp $(BIN_DATA) $(R_BUILD)
	cp $(SDL_DLL) $(R_BUILD)
	cp $(SDL_MIXER_DLL) $(R_BUILD)


build: $(program_exe)


clean:
	rm -fv $(O_BUILD)/*
	rm -fv $(R_BUILD)/*


setup:
	mkdir -p $(O_BUILD)
	mkdir -p $(R_BUILD)


delete:
	rm -rfv $(BUILD)