ZIG := zig build
ZIG += --summary all
#ZIG += --prefix ...


ZIG_BUILD := ./zig-out/bin


ROOT := ../../..

BUILD := $(ROOT)/build/ubuntu_sdl2

OUT := $(BUILD)/$(EXE)


#****************

build:
	$(ZIG)
	cp $(ZIG_BUILD)/* $(BUILD)


clean:
	rm -rfv $(BUILD)/*


setup:
	mkdir -p $(BUILD)


delete:
	rm -rfv $(BUILD)