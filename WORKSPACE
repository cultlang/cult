
workspace(name = "org_cultlang_cult")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

########################
# tao/pegtl:
http_archive(
    name = "pegtl",
    urls = ["https://github.com/taocpp/PEGTL/archive/2.7.1.zip"],
    strip_prefix = "PEGTL-2.7.1",
    sha256 = "1fce28859349f00ad7f932e608181eff01c982615f7e853c76055f2644f41cb3",
    build_file = "@//:bazel/libs/pegtl.BUILD.bazel",
)

########################
# OB1 shared load
git_repository(
    name = "ninja_ob1_bazel",
    commit = "b215b35863421072110638db7840ecb9de3382e0",
    remote = "https://github.com/OffByOneStudios/ob1-bazel",
)

load("@ninja_ob1_bazel//:bazel/deps.bzl", "ob1_deps")
ob1_deps()

load("@ninja_ob1_bazel//:bazel/setup.bzl", "ob1_setup")
ob1_setup()

