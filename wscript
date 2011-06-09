import sys
import os

APPNAME = 'ohMediaPlayer'
VERSION = '0.1'

top = '.'
out = 'build' + os.sep + sys.platform

def options(ctx):
    ctx.load('compiler_cxx')
    ctx.add_option('--ohNetHeaders', action='store', default='../ohNet/Upnp/Build/Include', help='Path to root of ohNet header files')
    ctx.add_option('--ohNetLibraries', action='store', default='../ohNet/Upnp/Build/Obj', help='Path to root of ohNet library binaries')
    ctx.add_option('--debug', action='store', default='true', help='Generate and use binaries with debugging support')

def configure(ctx):
    ctx.load('compiler_cxx')

    #Arrange include paths and store in ctx.env.HeaderPath
    hpath = ctx.path.find_node(ctx.options.ohNetHeaders)
    hpath = hpath.abspath()
    hpath = [hpath, hpath + os.path.sep + 'Cpp']
    ctx.env.HeaderPath = hpath

    #Arrange library paths and store in ctx.env.LibraryPath
    lpath = ctx.path.find_node(ctx.options.ohNetLibraries)
    lpath = lpath.abspath()

    if(ctx.options.debug == 'true'):
        debug = True
    else:
        debug = False

    if sys.platform == 'win32':
        lpath = lpath + os.sep + 'Windows'
        ctx.env.Libraries = ['Ws2_32', 'Iphlpapi']
        ctx.env.Defines = ['DllExport=__declspec(dllexport)', 'DllExportClass=']
        ctx.env.CxxFlags = ['/EHsc', '/FR']
        if(debug):
            ctx.env.CxxFlags += ['/MTd', '/Od', '/Zi']
            ctx.env.LinkFlags += ['/debug']

    elif sys.platform == 'linux2':
        lpath = lpath + os.sep + 'Posix'
        ctx.env.Libraries = ['pthread']
        ctx.env.Defines = ['DllExport=__attribute__ ((visibility(\"default\")))', 'DllExportClass=__attribute__ ((visibility(\"default\")))']
        ctx.env.CxxFlags += ['-Wall', '-Werror', '-pipe', '-fexceptions']
        if(debug):
            ctx.env.CxxFlags += ['-g']
    else:
        ctx.fatal("Unsupported build platform {0}".format(os.sys.platform))

        
    if(debug): 
        lpath = lpath + os.sep + 'Debug'
    else:
        lpath = lpath + os.sep + 'Release'

    ctx.env.LibraryPath = lpath

    #Let user know about selected paths
    print 'HeaderPath: {0}'.format(ctx.env.HeaderPath)
    print 'LibraryPath: {0}'.format(ctx.env.LibraryPath)

    #Ensure those directories actually exist
    ctx.find_file('.', ctx.env.HeaderPath)
    ctx.find_file('.', ctx.env.LibraryPath)



def build(ctx):
    ctx.program(
        source      = ['main.cpp', 'OpenHome/Product.cpp'],
        target      = 'ohMediaPlayer',
        includes    = ctx.env.HeaderPath,
        defines     = ctx.env.Defines,
        stlib       = ['ohNetCore', 'ohNetDevices', 'TestFramework'],
        stlibpath   = ctx.env.LibraryPath,
        lib         = ctx.env.Libraries,
        cxxflags    = ctx.env.CxxFlags,
        linkflags   = ctx.env.LinkFlags
        )

