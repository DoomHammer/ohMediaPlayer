import sys
import os

APPNAME = 'ohMediaPlayer'
VERSION = '0.1'

top = '.'
out = 'build' + os.sep + sys.platform

def options(ctx):
    ctx.load('compiler_cxx')
    ctx.add_option('--ohNetHeaders', action='store', default='../ohNet/Build/Include', help='Path to root of ohNet header files')
    ctx.add_option('--ohNetLibraries', action='store', default='../ohNet/Build/Obj', help='Path to root of ohNet library binaries')
    ctx.add_option('--vlcHeaders', action='store', default='../vlc-1.1.10/include', help='Path to root of vlc header files')
    ctx.add_option('--disableVlc', action='store_true', default=False, help='Should VLC support be built')
    ctx.add_option('--debug', action='store', default='true', help='Generate and use binaries with debugging support')
    ctx.add_option('--prefix', action='store', default='install', help='Installation prefix')

def configure(ctx):
    if sys.platform == 'win32':
        ctx.env['MSVC_TARGETS'] = ['x86']
        ctx.load('msvc')
    else:
        ctx.load('compiler_cxx')

    #Arrange include paths and store in ctx.env.HeaderPath
    ohNetHeaders = ctx.path.find_node(ctx.options.ohNetHeaders)
    ohNetHeaders = ohNetHeaders.abspath()

    curpath = ctx.path.find_node('.')
    curpath = curpath.abspath()

    hpath = [ohNetHeaders, curpath]
    ctx.env.INCLUDES_MEDIA = hpath

    ctx.env.DISABLEVLC = ctx.options.disableVlc
    if ctx.options.disableVlc == False:
        vlcHeaders = ctx.path.find_node(ctx.options.vlcHeaders)
        vlcHeaders = vlcHeaders.abspath()
        ctx.env.INCLUDES_VLC = [vlcHeaders]
        ctx.check(header_name='vlc/vlc.h')

    #Arrange library paths and store in ctx.env.LibraryPath
    ohNetLibraries = ctx.path.find_node(ctx.options.ohNetLibraries)
    ohNetLibraries = ohNetLibraries.abspath()

    vlcLibraries = None

    if(ctx.options.debug == 'true'):
        debug = True
    else:
        debug = False

    if sys.platform == 'win32':
        ohNetLibraries = ohNetLibraries + os.sep + 'Windows'
        if ctx.options.disableVlc == False:
            vlcLibraries = ctx.path.find_node('./Renderers/Vlc')
            vlcLibraries = vlcLibraries.abspath()
            ctx.env.LIB_VLC = ['libvlc']
        ctx.env.LIB_MEDIA = ['Ws2_32', 'Iphlpapi']
        ctx.env.CXXFLAGS_MEDIA = ['/EHsc', '/FR']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['/MTd', '/Od', '/Zi']
            ctx.env.LINKFLAGS_MEDIA += ['/debug']
        else:
            ctx.env.CXXFLAGS_MEDIA += ['/MT', '/Ox']

    elif sys.platform == 'linux2':
        ohNetLibraries = ohNetLibraries + os.sep + 'Posix'
        if ctx.options.disableVlc == False:
            vlcLibraries = ctx.path.find_node('../vlc-1.1.10/src/.libs')
            vlcLibraries = vlcLibraries.abspath()
            ctx.env.LIB_VLC = ['vlc', 'vlccore']
        ctx.env.LIB_MEDIA = ['pthread']
        #ctx.env.CXXFLAGS_MEDIA += ['-Wall', '-Werror', '-pipe', '-fexceptions']
        ctx.env.CXXFLAGS_MEDIA += ['-Wall', '-pipe', '-fexceptions']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['-g']

    elif sys.platform == 'darwin':
        ohNetLibraries = ohNetLibraries + os.sep + 'Mac'
        if ctx.options.disableVlc == False:
            vlcLibraries = ctx.path.find_node('../../../Applications/VLC.app/Contents/MacOS/lib')
            print vlcLibraries
            vlcLibraries = vlcLibraries.abspath()
            print vlcLibraries
            ctx.env.LIB_VLC = ['vlc']
        ctx.env.LIB_MEDIA = ['pthread']
        ctx.env.CXXFLAGS_MEDIA += ['-Werror', '-pipe', '-fexceptions']
        ctx.env.LINKFLAGS_MEDIA += ['-framework', 'CoreFoundation', '-framework', 'SystemConfiguration']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['-g']
    else:
        ctx.fatal("Unsupported build platform {0}".format(os.sys.platform))

        
    if(debug): 
        ohNetLibraries = ohNetLibraries + os.sep + 'Debug'
    else:
        ohNetLibraries = ohNetLibraries + os.sep + 'Release'

    ctx.env.LIBPATH_MEDIA = [ohNetLibraries]
    if ctx.options.disableVlc == False:
        ctx.env.LIBPATH_VLC = [vlcLibraries]

    #Let user know about selected paths
    print 'INCLUDES: {0}'.format(ctx.env.INCLUDES_MEDIA)
    print 'LIBPATH: {0}'.format(ctx.env.LIBPATH_MEDIA)

    #Ensure those directories actually exist
    #ctx.find_file('.', ctx.env.INCLUDES_MEDIA)
    #ctx.find_file('.', ctx.env.LIBPATH_MEDIA)

    ctx.write_config_header('config.h')


def build(ctx):
    ctx.install_files('${PREFIX}/include', [
            'OpenHome/Store.h',
        ], relative_trick=True)
    
    ctx.install_files('${PREFIX}/share/Tests/TestStore', [
            'OpenHome/Tests/TestStore/defaults0.txt',
            'OpenHome/Tests/TestStore/defaultsString0.txt',
        ], relative_trick=False)

    ctx.stlib(
        source = [
            'OpenHome/Store.cpp',
            'OpenHome/MurmurHash3.cpp'
        ],
        target = 'ohPersist',
        use    = 'MEDIA',
        includes = ctx.env.INCLUDES_MEDIA
        )

    ctx.program(
        source      = [
            'OpenHome/Tests/TestStore.cpp'
        ],
        includes    = ctx.env.INCLUDES_MEDIA,
        target      = 'TestStore',
        install_path= '${PREFIX}/bin/Tests',
        stlib       = ['ohNetCore', 'TestFramework'],
        use         = ['MEDIA', 'ohPersist']
        )

    ctx.stlib(
        source = [
            'OpenHome/Media/Product.cpp', 
            'OpenHome/Media/Info.cpp',
            'OpenHome/Media/Time.cpp',
            'OpenHome/Media/Volume.cpp',
            'OpenHome/Media/Playlist.cpp',
            'OpenHome/Media/Player.cpp',
            'OpenHome/Media/Standard.cpp',
            'OpenHome/Media/Source.cpp',
            'OpenHome/Media/SourcePlaylist.cpp'
        ],
        target = 'ohMedia',
        use    = ['MEDIA', 'ohPersist'],
        includes = ctx.env.INCLUDES_MEDIA
        )

    ctx.stlib(
        source      = [
            'Renderers/Dummy/Dummy.cpp'
            ],
        includes    = ctx.env.INCLUDES_MEDIA,
        target      = 'rendererDummy',
        )

    uses = ['ohMedia', 'rendererDummy']

    if ctx.env.DISABLEVLC == False:
        ctx.stlib(
            source      = [
                'Renderers/Vlc/Vlc.cpp'
                ],
            includes    = ctx.env.INCLUDES_MEDIA,
            target      = 'rendererVlc',
            use         = ['VLC']
            )

        uses.append('rendererVlc')

    ctx.program(
        source      = [
            'Renderers/main.cpp',
            ],
        includes    = ctx.env.INCLUDES_MEDIA,
        target      = 'ohMediaPlayer',
        stlib       = ['ohNetCore', 'ohNetDevices', 'TestFramework'],
        use         = uses
        )
