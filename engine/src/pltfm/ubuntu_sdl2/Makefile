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
X11 := -lX11 -lxcb

ALL_LFLAGS := $(SDL2) $(SDL_AUDIO) $(OPENGL) $(THREAD) $(X11)

root       := ../../../..

engine     := $(root)/engine
build      := $(engine)/build/ubuntu_sdl2
src        := $(engine)/src

pltfm       := $(src)/pltfm/ubuntu_sdl2
pltfm_imgui := $(src)/pltfm/imgui_sdl2_ogl3

libs   := $(root)/libs

exe := exe_sdl_ogl

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
io_test_c += $(filesystem_h)
io_test_c += $(io_test_res)/asset_sizes.cpp

#************


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


#*** main_o ***

main_o := $(src)/pltfm/main_o

main_o_c := $(main_o)/main_o_sdl2.cpp
main_o_c += $(game_state_c)
main_o_c += $(io_test_c)
main_o_c += $(image_c)
main_o_c += $(ascii_image_c)
main_o_c += $(sdl_span_c)
main_o_c += $(sdl_alloc_c)
main_o_c += $(sdl_input_c)
main_o_c += $(sdl_window_c)
main_o_c += $(sdl_audio_c)
main_o_c += $(sdl_filesystem_c)
main_o_c += $(sdl_stb_libs_c)
main_o_c += $(math_c)
main_o_c += $(datetime_c)

#************


#*** main cpp ***

main_c := $(pltfm)/engine_main_ubuntu.cpp
main_o := $(build)/main.o
obj    := $(main_o)

main_dep := $(io_test_h)
main_dep += $(imgui_include_h)
main_dep += $(ui_h)
main_dep += $(datetime_h)
main_dep += $(main_o_c)

MAIN_LFLAGS := $(SDL2) $(OPENGL) $(X11)

#************


#*** imgui cpp ***

imgui_c := $(pltfm_imgui)/imgui_o.cpp
imgui_o := $(build)/imgui.o
obj     += $(imgui_o)

imgui_dep := $(imgui_options_h)

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