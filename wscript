top = "."

def options(opt):
    opt.load("compiler_c compiler_cxx")

def configure(cfg):

    cfg.setenv("debug")
    cfg.load("compiler_c compiler_cxx")
    cfg.env.append_value("CFLAGS", ["-g"])
    cfg.env.append_value("CXXFLAGS", ["-g"])

    cfg.setenv("release")
    cfg.load("compiler_c compiler_cxx")
    cfg.env.append_value("CFLAGS", ["-O2"])
    cfg.env.append_value("CXXFLAGS", ["-O2"])
    cfg.env.append_value("DEFINES", ["NDEBUG"])

def build(bld):
    if not bld.variant:
        bld.fatal('Try adding "_debug" to your command')

    bld.recurse("src")

    bld.program(

        source = [
            "example/main.cpp",
            ],

        target = "vfmd-example",
        use = "src",
        includes = "src"
        )

# Code for adding build_debug  build_release modes

from waflib.Build import BuildContext, CleanContext, \
    InstallContext, UninstallContext

modes = ["debug", "release"]
for x in modes:
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context','').lower()
        class tmp(y):
            cmd = name + '_' + x
            variant = x