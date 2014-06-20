top = "."

def options(opt):
    opt.load("compiler_c compiler_cxx")
    opt.add_option('--for-ruby', action='store_true', default=False,
                   help='Build a Ruby module')
    opt.add_option('--ruby-include', action='store', default="",
                   help='Ruby include dir (with ruby.h)')
    opt.add_option('--ruby-arch-include', action='store', default="",
                   help='Ruby arch include dir (with ruby/config.h)')

def configure(cfg):
    cfg.setenv("debug")
    cfg.load("compiler_c compiler_cxx")
    cfg.env.append_value("CFLAGS", ["-g"])
    cfg.env.append_value("CXXFLAGS", ["-g"])

    cfg.setenv("release")
    cfg.load("compiler_c compiler_cxx")
    cfg.env.append_value("CFLAGS", ["-O2", "-fPIC"])
    cfg.env.append_value("CXXFLAGS", ["-O2", "-fPIC"])
    cfg.env.append_value("DEFINES", ["NDEBUG"])

    for_ruby = cfg.options.for_ruby
    if (for_ruby):
        import os
        if (cfg.options.ruby_include == "" or
            cfg.options.ruby_arch_include == ""):
            print "Warning: Cannot build Ruby module without --ruby-include and --ruby-arch-include, " \
                  "so ignoring --with-ruby."
            for_ruby = False
        else:
            if (not (os.path.isfile(cfg.options.ruby_include + "/ruby.h"))):
                print "Warning: ruby.h not found at '" + cfg.options.ruby_include + "', so ignoring --with-ruby."
                for_ruby = False
            if (not (os.path.isfile(cfg.options.ruby_arch_include + "/ruby/config.h"))):
                print "Warning: ruby/config.h not found at '" + cfg.options.ruby_arch_include + "', so ignoring --with-ruby."
                for_ruby = False
    cfg.setenv("debug")
    if (for_ruby):
        cfg.env.FOR_RUBY = True
        cfg.env.RUBY_INCLUDE = cfg.options.ruby_include
        cfg.env.RUBY_ARCH_INCLUDE = cfg.options.ruby_arch_include
        cfg.find_program('swig', var='SWIG')
    else:
        cfg.env.FOR_RUBY = False
    cfg.setenv("release")
    if (for_ruby):
        cfg.env.FOR_RUBY = True
        cfg.env.RUBY_INCLUDE = cfg.options.ruby_include
        cfg.env.RUBY_ARCH_INCLUDE = cfg.options.ruby_arch_include
        cfg.find_program('swig', var='SWIG')
    else:
        cfg.env.FOR_RUBY = False

def build(bld):
    if not bld.variant:
        bld.fatal('Try adding "_debug" to your command')

    bld.recurse("src")

    # Build vfmd command
    bld.program(

        source = [
            "cmd/main.cpp",
            ],

        target = "vfmd",
        use = "src",
        includes = "src",
        name = "vfmd-cmd"
        )
    bindir = "./" + bld.variant + "/bin"
    bld(rule = "mkdir -p " + bindir + " && cp ${SRC} ${TGT}",
        source = "vfmd",
        target = "../../" + bld.variant + "/bin/vfmd",
        depends = "vfmd-cmd")

    # Build ruby module
    if bld.env.FOR_RUBY:
        bld.add_group()
        swig_interface = "src/use_vfmd.i"
        swig_wrapper = "vfmd_wrap.cxx"
        bld(rule = "${SWIG} -c++ -ruby -module vfmd -w362 -o ${TGT} ${SRC}",
            source = swig_interface,
            target = swig_wrapper,
            includes = "src"
            )
        # Ignoring the following SWIG warnings:
        #    Warning 362: operator= ignored
        bld.shlib(
            source = [ swig_wrapper ],
            target = "vfmd",
            use = "src",
            includes = [ "src", bld.env.RUBY_INCLUDE, bld.env.RUBY_ARCH_INCLUDE ],
            name = "vfmd-so"
            )
        rubylibdir = "./" + bld.variant + "/lib/ruby/use_vfmd"
        bld(rule = "mkdir -p " + rubylibdir + " && cp ${SRC} ${TGT}",
            source = "libvfmd.so",
            target = "../../" + bld.variant + "/lib/ruby/use_vfmd/vfmd.so",
            depends = "vfmd-so")


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
