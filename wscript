APPNAME = 'ohMediaPlayer'
VERSION = '0.1'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_cxx')

def configure(ctx):
    ctx.load('compiler_cxx')

def build(bld):
    bld.program(
        source      = ['main.cpp', 'OpenHome/Product.cpp'],
        target      = 'ohMediaPlayer',
        includes    = ['../ohNet/Upnp/Build/Include', '../ohNet/Upnp/Build/Include/Cpp'],
        defines     = ['DllExport=__attribute__ ((visibility(\"default\")))',
                       'DllExportClass=__attribute__ ((visibility(\"default\")))'],
        stlib       = ['ohNetCore', 'ohNetDevices', 'TestFramework'],
        stlibpath   = '../../ohNet/Upnp/Build/Obj/Posix/Debug',
        lib         = ['pthread']
        )

