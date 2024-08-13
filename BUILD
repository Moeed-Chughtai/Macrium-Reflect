load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")


cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = ["//dependencies", "vhdx_manager"],
)

cc_library(
    name = "vhdx_manager",
    srcs = [
        "vhdx_manager.cpp",
    ],
    hdrs = [
        "vhdx_manager.h",
    ],
    visibility = ["//visibility:public"],
)