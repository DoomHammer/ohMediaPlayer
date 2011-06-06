APPNAME = 'ohMediaPlayer'
VERSION = '0.1'

top = '.'
out = 'build'

def options(ctx):
    ctx.load('compiler_cxx')
    ctx.add_option('--ohNetHeaders', action='store', default='../ohNet/Upnp/Build/Include', help='Path to root of ohNet header files')
    ctx.add_option('--ohNetLibraries', action='store', default='../ohNet/Upnp/Build/Obj', help='Path to root of ohNet library binaries')
    ctx.add_option('--debug', action='store', default='true', help='Generate and use binaries with debugging support')

def configure(ctx):
    ctx.load('compiler_cxx')

    hpath = ctx.path.find_node(ctx.options.ohNetHeaders)
    hpath = hpath.abspath()
    hpath = [hpath, hpath + '/Cpp']
    ctx.env.HeaderPath = hpath

    lpath = ctx.path.find_node(ctx.options.ohNetLibraries)
    lpath = lpath.abspath()
    if(ctx.options.debug):
        ctx.env.LibPath = lpath + '/Posix/Debug'
    else:
        ctx.env.LibPath = lpath + '/Posix/Release'

    print 'HeaderPath: ' 
    print ctx.env.HeaderPath
    print 'LibPath: ' 
    print ctx.env.LibPath

    ctx.find_file('.', ctx.env.HeaderPath)
    ctx.find_file('.', ctx.env.LibPath)


def build(ctx):
    print ctx.env.HeaderPath
    print ctx.env.LibPath

    ctx.program(
        source      = ['main.cpp', 'OpenHome/Product.cpp'],
        target      = 'ohMediaPlayer',
        includes    = ctx.env.HeaderPath,
        defines     = ['DllExport=__attribute__ ((visibility(\"default\")))',
                       'DllExportClass=__attribute__ ((visibility(\"default\")))'],
        stlib       = ['ohNetCore', 'ohNetDevices', 'TestFramework'],
        stlibpath   = ctx.env.LibPath,
        lib         = ['pthread']
        )

