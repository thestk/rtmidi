const std = @import("std");
const Build = std.Build;

pub fn build(b: *Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addStaticLibrary(.{
        .name = "rtmidi",
        .target = target,
        .optimize = optimize,
    });
    const t = lib.target_info.target;
    switch (t.os.tag) {
        .macos => {
            lib.addCSourceFiles(&.{
                "RtMidi.cpp",
                "rtmidi_c.cpp",
                }, &.{
                "-std=c++11",
                "-D__MACOSX_CORE__",
            });
            lib.linkFramework("CoreServices");
            lib.linkFramework("CoreAudio");
            lib.linkFramework("CoreMIDI");
            lib.linkFramework("CoreFoundation");
        },
        else => {
            lib.addCSourceFiles(&.{
                "RtMidi.cpp",
                "rtmidi_c.cpp",
                }, &.{
                "-std=c++11",
                "-D__LINUX_ALSA__",
            });
            lib.addIncludePath("/usr/include");
            lib.linkSystemLibrary("asoundlib");
        }
    }
    lib.linkLibCpp();
    lib.installHeader("rtmidi_c.h", "rtmidi/rtmidi_c.h");

    b.installArtifact(lib);
}
