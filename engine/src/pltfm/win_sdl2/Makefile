GPP := g++

GPP += -std=c++20
GPP += -mavx -mavx2 -mfma
GPP += -mwindows

GPP += -mconsole
#GPP += -DNDEBUG -O3

GPP += -DIMAGE_READ
GPP += -DIMAGE_WRITE
GPP += -DALLOC_COUNT

# directories
SDL := C:\SDL2\SDL2-2.30.5\x86_64-w64-mingw32
SDL_AUDIO := C:\SDL2\SDL2_mixer-2.8.0\x86_64-w64-mingw32

# include directories
INCSDL := $(SDL)\include
INCSDL2 := $(SDL)\include\SDL2
INCSDL_AUDIO := $(SDL_AUDIO)\include

# lib directories
LIBSDL := $(SDL)\lib
LIBSDL_AUDIO := $(SDL_AUDIO)\lib

# bin directories
BINSDL := $(SDL)\bin
BINSDL_AUDIO := $(SDL_AUDIO)\bin

# dll files
SDL_DLL := $(BINSDL)/SDL2.dll
SDL_AUDIO_DLL := $(BINSDL_AUDIO)/SDL2_mixer.dll

# include directory flags
ISDL := -I$(INCSDL) -I$(INCSDL2)
ISDL_AUDIO := -I$(INCSDL_AUDIO)

# link directory flags
LDSDL := -L$(LIBSDL)
LDSDL_AUDIO := -L$(LIBSDL_AUDIO)

# flags
LSDL := $(LDSDL) -lmingw32 -lsdl2 -lSDL2main -lgdi32
LSDL_AUDIO := $(LDSDL_AUDIO) -lSDL2_mixer
LDX11 := -limm32 -ld3d11 -ld3dcompiler -ldxgi



root       := ../../../..

engine     := $(root)/engine
build      := $(engine)/build/win_sdl2
build_o    := $(engine)/build_o/win_sdl2
src        := $(engine)/src

pltfm       := $(src)/pltfm/win_sdl2
pltfm_imgui := $(src)/pltfm/imgui_sdl2_dx11

libs   := $(root)/libs

exe := exe_sdl_dx11

program_exe := $(build)/$(exe)


#*** imgui ***

imgui_options_h := $(pltfm_imgui)/imgui_options.hpp

imgui_include_h := $(pltfm_imgui)/imgui_include.hpp
imgui_include_h += $(imgui_options_h)

#**************


#*** libs/util ***

util := $(libs)/util

types_h := $(util)/types.hpp

numeric_h := $(util)/numeric.hpp
numeric_h += $(types_h)

#************


#*** alloc_type ***

alloc_type := $(libs)/alloc_type

alloc_type_h := $(alloc_type)/alloc_type.hpp
alloc_type_h += $(types_h)

#*************


#*** memory_buffer ***

memory_buffer_h := $(util)/memory_buffer.hpp
memory_buffer_h += $(alloc_type_h)

#***********


#*** stb_libs ***

stb_libs := $(libs)/stb_libs

qsprintf_h := $(stb_libs)/qsprintf.hpp
stb_image_options_h := $(stb_libs)/stb_image_options.hpp

stb_libs_c := $(stb_libs)/stb_libs.cpp
stb_libs_c += $(stb_image_options_h)

#*************


#*** span ***

span := $(libs)/span

span_h := $(span)/span.hpp
span_h += $(memory_buffer_h)
span_h += $(stack_buffer_h)
span_h += $(qsprintf_h)

#************


#*** math ***

math := $(libs)/math

math_h := $(math)/math.hpp
math_h += $(types_h)

math_c := $(math)/math.cpp
math_c += $(math_h)

#************


#*** datetime ***

datetime := $(libs)/datetime

datetime_h := $(datetime)/datetime.hpp
datetime_h += $(types_h)

datetime_c := $(datetime)/datetime.cpp
datetime_c += $(datetime_h)

#***********


#*** image ***

image := $(libs)/image

image_h := $(image)/image.hpp
image_h += $(span_h)

image_c := $(image)/image.cpp
image_c += $(image_h)
image_c += $(numeric_h)
image_c += $(stb_image_options_h)

#*************


#*** ascii_image ***

ascii_image := $(libs)/ascii_image

ascii_image_h := $(ascii_image)/ascii_image.hpp
ascii_image_h += $(image_h)
ascii_image_h += $(span_h)
ascii_image_h += $(numeric_h)

ascii_image_c := $(ascii_image)/ascii_image.cpp
ascii_image_c += $(ascii_image_h)
ascii_image_c += $(ascii_image)/ascii_5.cpp
ascii_image_c += $(ascii_image)/ascii_joystick_8.cpp

#************


#*** io ***

io := $(libs)/io

input := $(io)/input

input_h := $(input)/input.hpp
input_h += $(types_h)
input_h += $(input)/keyboard_input.hpp
input_h += $(input)/mouse_input.hpp
input_h += $(input)/gamepad_input.hpp
input_h += $(input)/joystick_input.hpp

input_state_h := $(input)/input_state.hpp
input_state_h += $(input_h)

window_h := $(io)/window.hpp
audio_h := $(io)/audio.hpp
filesystem_h := $(io)/filesystem.hpp

#*************


#*** sdl2 ***

sdl2 := $(libs)/sdl2

sdl_include_h := $(sdl2)/sdl_include.hpp

sdl_span_c := $(sdl2)/sdl_span.cpp
sdl_span_c += $(span_h)
sdl_span_c += $(sdl_include_h)

sdl_alloc_c := $(sdl2)/sdl_alloc.cpp
sdl_alloc_c += $(alloc_type_h)
sdl_alloc_c += $(sdl_include_h)
sdl_alloc_c += $(span_h)

sdl_audio_c := $(sdl2)/sdl_audio.cpp
sdl_audio_c += $(audio_h)
sdl_audio_c += $(filesystem_h)
sdl_audio_c += $(numeric_h)
sdl_audio_c += $(alloc_type_h)
sdl_audio_c += $(sdl_include_h)

sdl_filesystem_c := $(sdl2)/sdl_filesystem.cpp
sdl_filesystem_c += $(filesystem_h)
sdl_filesystem_c += $(alloc_type_h)
sdl_filesystem_c += $(sdl_include_h)

sdl_input_c := $(sdl2)/sdl_input.cpp
sdl_input_c += $(input_state_h)
sdl_input_c += $(numeric_h)
sdl_input_c += $(sdl_include_h)

sdl_stb_libs_c := $(sdl2)/sdl_stb_libs.cpp
sdl_stb_libs_c += $(stb_libs_c)

sdl_window_c := $(sdl2)/sdl_window.cpp
sdl_window_c += $(window_h)
sdl_window_c += $(alloc_type_h)
sdl_window_c += $(sdl_include_h)

#************


#*** io_test ********

io_test := $(src)/io_test

io_test_app := $(io_test)/app
io_test_res := $(io_test)/res

io_test_h := $(io_test_app)/app.hpp

io_test_c := $(io_test_app)/app.cpp
io_test_c += $(io_test_h)
io_test_c += $(numeric_h)
io_test_c += $(ascii_image_h)

# assets.cpp
io_test_c += $(io_test_app)/assets.cpp
io_test_c += $(audio_h)
io_test_c += $(io_test_res)/asset_sizes.cpp

#*************


#*** game_punk ****

game_punk := $(root)/game_punk
game_punk_app := $(game_punk)/src/app
game_punk_bin := $(game_punk)/res/xbin

game_punk_h := $(game_punk_app)/app.hpp
game_punk_h += $(input_h)
game_punk_h += $(image_h)

game_punk_c := $(game_punk_app)/app.cpp
game_punk_c += $(game_punk_h)
game_punk_c += $(math_h)
game_punk_c += $(game_punk_app)/app_types.hpp

game_punk_c += $(game_punk_app)/assets.hpp
game_punk_c += $(filesystem_h)
game_punk_c += $(game_punk_bin)/bin_table.hpp

game_punk_c += $(game_punk_app)/memory.hpp


#*************


#*** game state ***

game_state := $(src)/game_state

game_punk_state_c := $(game_state)/game_punk_state.cpp
game_punk_state_c += $(game_punk_c)

game_state_h := $(game_state)/game_state.hpp
game_state_h += $(input_h)
game_state_h += $(image_h)

game_state_c := $(game_state)/game_state.cpp
game_state_c += $(game_state_h)
game_state_c += $(qsprintf_h)
game_state_c += $(numeric_h)
game_state_c += $(datetime_h)

game_state_c += $(game_punk_state_c)

#***************


#*** ui ***

ui := $(src)/ui

ui_h := $(ui)/ui.hpp
ui_h += $(imgui_h)
ui_h += $(qsprintf_h)
ui_h += $(alloc_type_h)

ui_h += $(ui)/diagnostics_window.hpp
ui_h += $(game_state_h)

ui_h += $(ui)/input_frames_window.hpp

#*************


#*** input_display ***

input_display := $(src)/input_display

input_display_h := $(input_display)/input_display.hpp
input_display_h += $(input_h)
input_display_h += $(image_h)

#***************


#*** main cpp ***

main_c := $(pltfm)/engine_main_win.cpp
main_o := $(build_o)/main.o
obj    := $(main_o)

main_dep := $(imgui_include_h)
main_dep += $(ui_h)
main_dep += $(io_test_h)

# main_o.cpp
main_dep += $(pltfm)/main_o.cpp
main_dep += $(game_state_c)
main_dep += $(io_test_c)
main_dep += $(image_c)
main_dep += $(ascii_image_c)
main_dep += $(sdl_span_c)
main_dep += $(sdl_alloc_c)
main_dep += $(sdl_input_c)
main_dep += $(sdl_window_c)
main_dep += $(sdl_audio_c)
main_dep += $(sdl_filesystem_c)
main_dep += $(sdl_stb_libs_c)
main_dep += $(math_c)
main_dep += $(datetime_c)

MAIN_IFLAGS := $(ISDL) $(ISDL_AUDIO)
MAIN_LFLAGS := $(LSDL) $(LSDL_AUDIO) $(LDX11)

#************


#*** imgui cpp ***

imgui_c := $(pltfm_imgui)/imgui_o.cpp
imgui_o := $(build_o)/imgui.o
obj     += $(imgui_o)

imgui_dep := $(imgui_options_h)

IMGUI_IFLAGS := $(ISDL)
IMGUI_LFLAGS := $(LSDL) $(LDX11)

#****************


#*** app ***

$(main_o): $(main_c) $(main_dep)
	@echo "  *** main ***"
	$(GPP) $(MAIN_IFLAGS) -o $@ -c $< $(MAIN_LFLAGS)


$(imgui_o): $(imgui_c) $(imgui_dep)
	@echo "  *** imgui ***"
	$(GPP) $(IMGUI_IFLAGS) -o $@ -c $< $(IMGUI_LFLAGS)

#**************


$(program_exe): $(obj)
	@echo "  *** program_exe ***"
	$(GPP) -o $@ $+ $(MAIN_LFLAGS)
	cp $(SDL_DLL) $(build)
	cp $(SDL_AUDIO_DLL) $(build)


build: $(program_exe)


run: build
	$(program_exe)
	@echo "\n"


clean:
	rm -rfv $(build)/*
	rm -rfv $(build_o)/*

setup:
	mkdir -p $(build)
	mkdir -p $(build_o)