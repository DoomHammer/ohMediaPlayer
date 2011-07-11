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
    ctx.add_option('--vlcHeaders', action='store', default='../vlc-1.1.10/include', help='Path to root of vlc header files')
    ctx.add_option('--debug', action='store', default='true', help='Generate and use binaries with debugging support')

def configure(ctx):
    if sys.platform == 'win32':
        ctx.env['MSVC_TARGETS'] = ['x86']
        ctx.load('msvc')
    else:
        ctx.load('compiler_cxx')

    #Arrange include paths and store in ctx.env.HeaderPath
    ohNetHeaders = ctx.path.find_node(ctx.options.ohNetHeaders)
    ohNetHeaders = ohNetHeaders.abspath()
   
    vlcHeaders = ctx.path.find_node(ctx.options.vlcHeaders)
    vlcHeaders = vlcHeaders.abspath()

    hpath = [ohNetHeaders, ohNetHeaders + os.path.sep + 'Cpp', vlcHeaders]
    ctx.env.INCLUDES_MEDIA = hpath

    #Arrange library paths and store in ctx.env.LibraryPath
    ohNetLibraries = ctx.path.find_node(ctx.options.ohNetLibraries)
    ohNetLibraries = ohNetLibraries.abspath()

    vlcLibraries = ctx.path.find_node('./OpenHome/Renderers/Vlc')
    vlcLibraries = vlcLibraries.abspath()

    if(ctx.options.debug == 'true'):
        debug = True
    else:
        debug = False

    if sys.platform == 'win32':
        ohNetLibraries = ohNetLibraries + os.sep + 'Windows'
        ctx.env.LIB_MEDIA = ['Ws2_32', 'Iphlpapi', 'libvlc']
        ctx.env.DEFINES_MEDIA = ['DllExport=__declspec(dllexport)', 'DllExportClass=']
        ctx.env.CXXFLAGS_MEDIA = ['/EHsc', '/FR', '/Gd']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['/MTd', '/Od', '/Zi']
            ctx.env.LINKFLAGS_MEDIA += ['/debug']
        else:
            ctx.env.CXXFLAGS_MEDIA += ['/MT', '/Ox']

    elif sys.platform == 'linux2':
        ohNetLibaries = ohNetLibaries + os.sep + 'Posix'
        ctx.env.LIB_MEDIA = ['pthread', 'vlc']
        ctx.env.DEFINES_MEDIA = ['DllExport=__attribute__ ((visibility(\"default\")))', 'DllExportClass=__attribute__ ((visibility(\"default\")))']
        ctx.env.CXXFLAGS_MEDIA += ['-Wall', '-Werror', '-pipe', '-fexceptions']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['-g']
    else:
        ctx.fatal("Unsupported build platform {0}".format(os.sys.platform))

        
    if(debug): 
        ohNetLibraries = ohNetLibraries + os.sep + 'Debug'
    else:
        ohNetLibraries = ohNetLibraries + os.sep + 'Release'

    ctx.env.LIBPATH_MEDIA = [ohNetLibraries, vlcLibraries]

    #Let user know about selected paths
    print 'INCLUDES: {0}'.format(ctx.env.INCLUDES_MEDIA)
    print 'LIBPATH: {0}'.format(ctx.env.LIBPATH_MEDIA)

    #Ensure those directories actually exist
    ctx.find_file('.', ctx.env.INCLUDES_MEDIA)
    ctx.find_file('.', ctx.env.LIBPATH_MEDIA)



def build(ctx):
    ctx.stlib(
        source = [
            'OpenHome/Product.cpp', 
            'OpenHome/Info.cpp',
            'OpenHome/Time.cpp',
            'OpenHome/Playlist.cpp',
            'OpenHome/Player.cpp',
            'OpenHome/Standard.cpp',
            'OpenHome/Source.cpp',
            'OpenHome/SourcePlaylist.cpp'
        ],
        target = 'ohMedia',
        use    = 'MEDIA',
        includes = ctx.env.INCLUDES_MEDIA
        )

    ctx.program(
        source      = [
            'OpenHome/Renderers/Dummy/main.cpp',
            'OpenHome/Renderers/Dummy/Dummy.cpp'
            ],
        includes    = ctx.env.INCLUDES_MEDIA,
        target      = 'ohMediaPlayerDummy',
        stlib       = ['ohNetCore', 'ohNetDevices', 'TestFramework'],
        use         = ['MEDIA', 'ohMedia']
        )

    ctx.program(
        source      = [
            'OpenHome/Renderers/Vlc/main.cpp',
            'OpenHome/Renderers/Vlc/Vlc.cpp'
            ],
        includes    = ctx.env.INCLUDES_MEDIA,
        target      = 'ohMediaPlayerVlc',
        stlib       = ['ohNetCore', 'ohNetDevices', 'TestFramework'],
        use         = ['MEDIA', 'ohMedia']
        )


