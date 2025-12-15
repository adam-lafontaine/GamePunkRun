const std = @import("std");

const root = "../../../..";
const engine = root ++ "/engine";
const src = engine ++ "/src";

const pltfm_imgui = src ++ "/pltfm/imgui_sdl2_ogl3";



const main_cpp = "./engine_main_ubuntu.cpp";
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


const cpp_libs = &[_][]const u8{
    "SDL2",
    "SDL2_mixer",
    "pthread",
    "tbb",
    "GL",
    "dl"
};


pub fn build(b: *std.Build) void 
{
    const optimize = b.option(std.builtin.OptimizeMode, "optimize", "Optimization mode") orelse .Debug;
    //const optimize = b.option(std.builtin.OptimizeMode, "optimize", "Optimization mode") orelse .ReleaseFast;


    const exe = b.addExecutable(.{
        .name = app_name,
        .root_module = b.createModule(.{
            .root_source_file = null,
            .target = b.graph.host,
            .optimize = optimize,
        }),
    });

    exe.addCSourceFiles(.{
        .files = &.{
            main_cpp,
            imgui_cpp
        },
        .flags = cpp_flags,
    });

    for (cpp_libs) |lib|
    {
        exe.linkSystemLibrary(lib);
    }
    
    exe.linkLibC();
    exe.linkLibCpp();

    b.installArtifact(exe);


    // zig build run            → run the game (with optional args)
    const run_step = b.step("run", "Run engine");
    const run_cmd = b.addRunArtifact(exe);
    if (b.args) |args| run_cmd.addArgs(args);
    run_step.dependOn(&run_cmd.step);

}