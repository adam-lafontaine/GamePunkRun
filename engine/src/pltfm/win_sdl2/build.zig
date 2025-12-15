const std = @import("std");

const root = "../../../..";

const sys_root = root ++ "/sys/Win";

const inc_all = sys_root ++ "/include";
const lib_all = sys_root ++ "/lib";
const bin_all = sys_root ++ "/bin";

const engine = root ++ "/engine";
const src = engine ++ "/src";

const pltfm_imgui = src ++ "/pltfm/imgui_sdl2_dx11";



const main_cpp = "./engine_main_win.cpp";
const imgui_cpp = pltfm_imgui ++ "/imgui_o.cpp";

const app_name = "engine";


const cpp_flags = &[_][]const u8{
    "-std=c++20",
    "-mavx",
    "-mavx2",
    "-mfma",
    "-DIMAGE_READ",
    "-DIMAGE_WRITE",
    "-DALLOC_COUNT",
    //"-DNDEBUG",
};

const inc_paths = &[_][]const u8{
    inc_all,
    inc_all ++ "/SDL2"
};

const lib_paths = &[_][]const u8{
    lib_all,
};

const cpp_libs = &[_][]const u8{
    "SDL2",
    "SDL2_mixer",
    //"imm32",
    "d3d11",
    "d3dcompiler",
    //"dxgi"
};


pub fn build(b: *std.Build) void 
{
    const optimize = b.option(std.builtin.OptimizeMode, "optimize", "Optimization mode") orelse .Debug;
    //const optimize = b.option(std.builtin.OptimizeMode, "optimize", "Optimization mode") orelse .ReleaseFast;


    const exe = b.addExecutable(.{
        .name = app_name,
        .root_module = b.createModule(.{
            .root_source_file = null,
            //.target = b.resolveTargetQuery(.{ .cpu_arch = .x86_64, .os_tag = .windows }),
            .target = b.graph.host,
            .optimize = optimize,
        }),
    });

    // keep console window
    //exe.subsystem = .Windows;

    exe.addCSourceFiles(.{
        .files = &.{
            main_cpp,
            imgui_cpp
        },
        .flags = cpp_flags,
    });

    for (inc_paths) |p|
    {
        exe.addIncludePath(b.path(p));
    }

    for (lib_paths) |p|
    {
        exe.addLibraryPath(b.path(p));
    }

    for (cpp_libs) |lib|
    {
        exe.linkSystemLibrary(lib);
    }
    
    exe.linkLibC();
    exe.linkLibCpp();

    b.installArtifact(exe);

    const copy_sdl = b.addInstallBinFile(
        b.path(bin_all ++ "/SDL2.dll"),
        "SDL2.dll"
    );
    b.getInstallStep().dependOn(&copy_sdl.step);

    const copy_sdl_mixer = b.addInstallBinFile(
        b.path(bin_all ++ "/SDL2_mixer.dll"),
        "SDL2_mixer2.dll"
    );
    b.getInstallStep().dependOn(&copy_sdl_mixer.step);


    // zig build run            → run the game (with optional args)
    const run_step = b.step("run", "Run engine");
    const run_cmd = b.addRunArtifact(exe);
    if (b.args) |args| run_cmd.addArgs(args);
    run_step.dependOn(&run_cmd.step);

}