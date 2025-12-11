const std = @import("std");

const main_cpp = "zig_main.cpp";


const cpp_flags = &[_][]const u8 {
    "-std=c++20",
    "-DNDEBUG",
};


pub fn build(b: *std.Build) void 
{
    const optimize = b.standardOptimizeOption(.{});


    const exe = b.addExecutable(.{
        .name = "hello",
        .root_module = b.createModule(.{
            .root_source_file = null,
            .target = b.graph.host,
            .optimize = optimize,
        }),
    });

    exe.addCSourceFiles(.{
        .files = &[_][]const u8{
            main_cpp,
        },
        .flags = cpp_flags,
    });

    exe.linkLibCpp();        // Required for <iostream>, std::cout, etc.
    b.installArtifact(exe);

    // Run step
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| 
    {
        run_cmd.addArgs(args);
    }
    b.step("run", "Run the app").dependOn(&run_cmd.step);
}