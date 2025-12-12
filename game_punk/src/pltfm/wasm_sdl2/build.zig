const std = @import("std");

const main_cpp = "./wasm_punk_main.cpp";

const app_name = "punk_run";


const root_dir = "../../..";
const res_dir = root_dir ++ "/res";
const bin_data = res_dir ++ "/xbin/punk_run.bin";


const cpp_flags = &[_][]const u8{
    "-std=c++20",
    "-DMATH_NO_SIMD",
    "-DGAME_PUNK_RELEASE",
    "-DAPP_ROTATE_90",
    "-DIMAGE_READ",
    "-DNDEBUG",
    "-O3",
    "-sASSERTIONS=1",
    "-sUSE_SDL=2",
    "-sUSE_SDL_MIXER=2",
    "-sUSE_OGG=1",
    "-sFETCH=1",
    "-sALLOW_MEMORY_GROWTH=1"
};


pub fn build(b: *std.Build) void 
{
    const optimize = b.option(std.builtin.OptimizeMode, "optimize", "Optimization mode") orelse .ReleaseFast;


    const exe = b.addExecutable(.{
        .name = app_name,
        .root_module = b.createModule(.{
            .root_source_file = null,
            .target = b.resolveTargetQuery(.{            // ← WebAssembly target
                .cpu_arch = .wasm32,
                .os_tag = .freestanding,
                .cpu_features_add = std.Target.wasm.featureSet(&.{ .atomics, .bulk_memory }),
            }),
            .optimize = optimize,
        }),
    });

    exe.entry = .disabled;
    exe.rdynamic = true;

    exe.addCSourceFiles(.{
        .files = &.{
            main_cpp,
        },
        .flags = cpp_flags,
    });
    
    exe.linkSystemLibrary("c");
    exe.linkLibCpp();

    b.installArtifact(exe);


    // Copy punk_run.bin next to the exe in zig-out/bin
    const copy_data = b.addInstallBinFile(
        b.path(bin_data),
        "punk_run.bin",
    );
    b.getInstallStep().dependOn(&copy_data.step);
    
    
}

// zig build -Doptimize=Debug          # full debug symbols, no -O3, assertions on
// zig build -Doptimize=ReleaseSafe    # safe release (keeps bounds checks)
// zig build -Doptimize=ReleaseFast    # your current -O3 mode (default above)