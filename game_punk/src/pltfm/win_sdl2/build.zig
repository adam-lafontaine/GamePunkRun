const std = @import("std");

const main_cpp = "./win_sdl2_punk_main.cpp";

const app_name = "punk_run";

const root_dir = "../../../..";
const game_root = root_dir ++ "/game_punk";
const sys_root = root_dir ++ "/sys/Win";

const res_dir = game_root ++ "/res";
const bin_data = res_dir ++ "/xbin/punk_run.bin";

const inc_all = sys_root ++ "/include";
const lib_all = sys_root ++ "/lib";
const bin_all = sys_root ++ "/bin";

const icon_rc = "punk.rc";


const cpp_flags = &[_][]const u8{
    "-std=c++20",
    "-mavx",
    "-mavx2",
    "-mfma",
    "-DNDEBUG",
    "-O3",
    "-DGAME_PUNK_RELEASE",
    "-DAPP_ROTATE_90",
    "-DIMAGE_READ",
};

const inc_paths = &[_][]const u8{
    inc_all,
};

const lib_paths = &[_][]const u8{
    lib_all,
};

const cpp_libs = &[_][]const u8{
    "SDL2",
    "SDL2_mixer",
    //"pthread",
    //"tbb"
};


pub fn build(b: *std.Build) void 
{
    const optimize = b.option(std.builtin.OptimizeMode, "optimize", "Optimization mode") orelse .ReleaseFast;


    const exe = b.addExecutable(.{
        .name = app_name,
        .root_module = b.createModule(.{
            .root_source_file = null,
            .target = b.resolveTargetQuery(.{ .cpu_arch = .x86_64, .os_tag = .windows }),
            .optimize = optimize,
        }),
    });

    exe.subsystem = .Windows;

    exe.addCSourceFiles(.{
        .files = &.{
            main_cpp,
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


    exe.addWin32ResourceFile(.{
        .file = b.path(icon_rc)
    });

    b.installArtifact(exe);

    const copy_sdl = b.addInstallBinFile(
        b.path(bin_all ++ "/SDL2.dll"),
        "SDL2.dll"
    );
    b.getInstallStep().dependOn(&copy_sdl.step);

    const copy_sdl_mixer = b.addInstallBinFile(
        b.path(bin_all ++ "/SDL2_mixer.dll"),
        "SDL2_mixer.dll"
    );
    b.getInstallStep().dependOn(&copy_sdl_mixer.step);

    // Copy punk_run.bin next to the exe in zig-out/bin
    const copy_data = b.addInstallBinFile(
        b.path(bin_data),
        "punk_run.bin",
    );
    b.getInstallStep().dependOn(&copy_data.step);

    // zig build run
    const run_step = b.step("run", "Run");
    const run_cmd = b.addRunArtifact(exe);
    if (b.args) |args| run_cmd.addArgs(args);
    run_step.dependOn(&run_cmd.step);    
}

// zig build -Doptimize=Debug          # full debug symbols, no -O3, assertions on
// zig build -Doptimize=ReleaseSafe    # safe release (keeps bounds checks)
// zig build -Doptimize=ReleaseFast    # your current -O3 mode (default above)