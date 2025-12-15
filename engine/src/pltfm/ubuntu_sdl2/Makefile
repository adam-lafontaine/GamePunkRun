GPP := g++-11 -std=c++20 -mavx -mavx2 -mfma

#GPP += -DNDEBUG -O3

GPP += -DIMAGE_READ
GPP += -DIMAGE_WRITE
GPP += -DALLOC_COUNT

NO_FLAGS := 
SDL2 := `sdl2-config --cflags --libs`
SDL_AUDIO := -lSDL2_mixer
OPENGL := -lGL -ldl
THREAD := -lpthread -ltbb

ALL_LFLAGS := $(SDL2) $(SDL_AUDIO) $(OPENGL) $(THREAD)

root       := ../../../..

engine     := $(root)/engine
build      := $(engine)/build/ubuntu_sdl2
src        := $(engine)/src

pltfm       := $(src)/pltfm/ubuntu_sdl2
pltfm_imgui := $(src)/pltfm/imgui_sdl2_ogl3

exe := exe_sdl_ogl

program_exe := $(build)/$(exe)


#*** main cpp ***

main_c := $(pltfm)/engine_main_ubuntu.cpp
main_o := $(build)/main.o
obj    := $(main_o)

main_dep := 

MAIN_LFLAGS := $(SDL2) $(OPENGL)

#************


#*** imgui cpp ***

imgui_c := $(pltfm_imgui)/imgui_o.cpp
imgui_o := $(build)/imgui.o
obj     += $(imgui_o)

imgui_dep := 

IMGUI_LFLAGS := $(SDL2) $(OPENGL)

#****************


#*** engine ***


$(main_o): $(main_c) $(main_dep)
	@echo "\n  main"
	$(GPP) -o $@ -c $< $(MAIN_LFLAGS)


$(imgui_o): $(imgui_c) $(imgui_dep)
	@echo "\n  imgui"
	$(GPP) -o $@ -c $< $(IMGUI_LFLAGS)

#**************


$(program_exe): $(obj)
	@echo "\n  program_exe"
	$(GPP) -o $@ $+ $(ALL_LFLAGS)


build: $(program_exe)


run: build
	$(program_exe)
	@echo "\n"


clean:
	rm -rfv $(build)/*

setup:
	mkdir -p $(build)