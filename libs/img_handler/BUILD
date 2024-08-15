load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "enums",
    hdrs = ["enums.h"],
    deps = ["//dependencies"]
)

cc_library(
    name = "img_handler_lib",
    hdrs = ["file_struct.h", "metadata.h"],
    deps = [":enums"]
)

cc_library(
    name = "img_handler",
    srcs = ["img_handler.cpp"],
    hdrs = ["img_handler.h"],
    deps = ["//libs/file_handler:file_handler", ":img_handler_lib"],
    visibility = ["//visibility:public"]
)