const std = @import("std");

const main_cpp = "../ubuntu_sdl2/ubuntu_sdl2_punk_main.cpp";

const app_name = "punk_run";


const cpp_flags = &[_][]const u8 {
    "-std=c++20",
    "-mavx",
    "-mavx2",
    "-mfma",
    "-DGAME_PUNK_RELEASE",
    "-DAPP_ROTATE_90",
    "-DNDEBUG",
    "-O3",
    "-DIMAGE_READ",
};


pub fn build(b: *std.Build) void 
{
    const optimize = b.option(std.builtin.OptimizeMode, "optimize", "Optimization mode") orelse .ReleaseFast;


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
        },
        .flags = cpp_flags,
    });

    exe.linkSystemLibrary("SDL2");
    exe.linkSystemLibrary("SDL2_mixer");
    exe.linkLibC();
    exe.linkLibCpp();        // Required for <iostream>, std::cout, etc.

    b.installArtifact(exe);

    // zig build run
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| 
    {
        run_cmd.addArgs(args);
    }
    b.step("run", "Run the app").dependOn(&run_cmd.step);
}

// zig build -Doptimize=Debug          # full debug symbols, no -O3, assertions on
// zig build -Doptimize=ReleaseSafe    # safe release (keeps bounds checks)
// zig build -Doptimize=ReleaseFast    # your current -O3 mode (default above)