import sys
import os

APPNAME = 'ohMediaPlayer'
VERSION = '0.0.1'

top = '.'
out = 'build' + os.sep + sys.platform

sys.path.append('waftools')

def find_rel_or_abs(ctx, node):
    ret = ctx.path.find_node(node)
    if ret is None:
        ret = ctx.root.find_node(node)
    return ret

def find_ohnet_libs(ctx, lib, platform):
    ohNetLibrariesPath = os.path.join(lib, platform, ctx.options.debugmode)
    ohNetLibraries = find_rel_or_abs(ctx, ohNetLibrariesPath)
    if ohNetLibraries is None:
        ohNetLibrariesPath = lib
        ohNetLibraries = find_rel_or_abs(ctx, ohNetLibrariesPath)
    return ohNetLibraries.abspath()

def options(ctx):
    ctx.load('compiler_cxx')
    ctx.add_option('--ohNetHeaders', action='store', default='../ohNet/Build/Include', help='Path to root of ohNet header files')
    ctx.add_option('--ohNetLibraries', action='store', default='../ohNet/Build/Obj', help='Path to root of ohNet library binaries')
    ctx.add_option('--ohNetGeneratedHeaders', action='store', default='../ohNetGenerated/Build/Include', help='Path to root of ohNetGenerated header files')
    ctx.add_option('--ohNetGeneratedLibraries', action='store', default='../ohNetGenerated/Build/Obj', help='Path to root of ohNetGenerated library binaries')
    ctx.add_option('--vlcHeaders', action='store', default='/usr/include', help='Path to root of vlc header files')
    ctx.add_option('--disableVlc', action='store_true', default=False, help='Should VLC support be built')
    ctx.add_option('--civetwebHeaders', action='store', default='../civetweb/include', help='Path to root of civetweb header files')
    ctx.add_option('--civetwebLibraries', action='store', default='../civetweb', help='Path to root of civetweb library binaries')
    ctx.add_option('--jsonhandleHeaders', action='store', default='../jsonhandle/src', help='Path to root of jsonhandle header files')
    ctx.add_option('--jsonhandleLibraries', action='store', default='../jsonhandle' + os.sep + out, help='Path to root of jsonhandle library binaries')
    ctx.add_option('--debug', action='store_const', const='Debug', default='Release', dest='debugmode', help='Generate and use binaries with debugging support')
    ctx.add_option('--release', action='store_const', const='Release', default='Release', dest='debugmode', help='Generate and use binaries without debugging support')
    ctx.add_option('--prefix', action='store', default='install', help='Installation prefix')

def configure(ctx):
    if sys.platform == 'win32':
        ctx.env['MSVC_TARGETS'] = ['x86']
        ctx.load('msvc')
    else:
        ctx.load('compiler_cxx')
    ctx.load('csr2h', tooldir='./waftools')

    #Arrange include paths and store in ctx.env.HeaderPath
    ohNetHeaders = find_rel_or_abs(ctx, ctx.options.ohNetHeaders)
    ohNetHeaders = ohNetHeaders.abspath()

    ohNetGeneratedHeaders = find_rel_or_abs(ctx, ctx.options.ohNetGeneratedHeaders)
    ohNetGeneratedHeaders = ohNetGeneratedHeaders.abspath()

    curpath = find_rel_or_abs(ctx, '.')
    curpath = curpath.abspath()

    hpath = [ohNetHeaders, ohNetGeneratedHeaders, curpath, top]
    ctx.env.INCLUDES_MEDIA = hpath

    vlcHeaders = find_rel_or_abs(ctx, ctx.options.vlcHeaders)
    if not vlcHeaders:
        print 'VLC headers not found.'
        ctx.options.disableVlc = True
    ctx.env.DISABLEVLC = ctx.options.disableVlc
    if ctx.options.disableVlc == False:
        vlcHeaders = vlcHeaders.abspath()
        ctx.env.INCLUDES_VLC = [vlcHeaders]
        ctx.check(header_name='vlc/vlc.h')
        print 'Using VLC headers from %s' % vlcHeaders

    civetwebHeaders = find_rel_or_abs(ctx, os.path.join(ctx.options.civetwebHeaders))
    civetwebHeaders = civetwebHeaders.abspath()

    jsonhandleHeaders = find_rel_or_abs(ctx, os.path.join(ctx.options.jsonhandleHeaders))
    jsonhandleHeaders = jsonhandleHeaders.abspath()

    ctx.env.INCLUDES_WEB = [civetwebHeaders, jsonhandleHeaders]

    vlcLibraries = None

    if(ctx.options.debugmode == 'Debug'):
        debug = True
    else:
        debug = False

    civetwebLibraries = find_rel_or_abs(ctx, os.path.join(ctx.options.civetwebLibraries))
    civetwebLibraries = civetwebLibraries.abspath()

    jsonhandleLibraries = find_rel_or_abs(ctx, os.path.join(ctx.options.jsonhandleLibraries))
    jsonhandleLibraries = jsonhandleLibraries.abspath()

    ctx.env.LIB_WEB = ['jsonhandle', 'civetweb', 'pthread', 'dl']

    if sys.platform == 'win32':
        ohNetPlatform = 'Windows'
        if ctx.options.disableVlc == False:
            vlcLibraries = find_rel_or_abs(ctx, './Renderers/Vlc')
            vlcLibraries = vlcLibraries.abspath()
            ctx.env.LIB_VLC = ['libvlc']
        ctx.env.LIB_MEDIA = ['Ws2_32', 'Iphlpapi']
        ctx.env.CXXFLAGS_MEDIA = ['/EHsc', '/FR']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['/MTd', '/Od', '/Zi']
            ctx.env.CXXFLAGS_WEB += ['/MTd', '/Od', '/Zi']
            ctx.env.LINKFLAGS_MEDIA += ['/debug']
        else:
            ctx.env.CXXFLAGS_MEDIA += ['/MT', '/Ox']
            ctx.env.CXXFLAGS_WEB += ['/MT', '/Ox']

    elif sys.platform == 'linux2':
        ohNetPlatform = 'Posix'
        if ctx.options.disableVlc == False:
            vlcLibraries = find_rel_or_abs(ctx, '/usr/lib')
            vlcLibraries = vlcLibraries.abspath()
            ctx.env.LIB_VLC = ['vlc', 'vlccore']

        ctx.env.LIB_MEDIA = ['pthread']
        #ctx.env.CXXFLAGS_MEDIA += ['-Wall', '-Werror', '-pipe', '-fexceptions']
        ctx.env.CXXFLAGS_MEDIA += ['-Wall', '-pipe', '-fexceptions', '-std=c++11']
        ctx.env.CXXFLAGS_WEB += ['-Wall', '-pipe', '-fexceptions', '-std=c++11']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['-g']

    elif sys.platform == 'darwin':
        ohNetPlatform = 'Mac-x64'
        if ctx.options.disableVlc == False:
            vlcLibraries = find_rel_or_abs(ctx, '../../../Applications/VLC.app/Contents/MacOS/lib')
            print vlcLibraries
            vlcLibraries = vlcLibraries.abspath()
            print vlcLibraries
            ctx.env.LIB_VLC = ['vlc']
        ctx.env.LIB_MEDIA = ['pthread']
        ctx.env.CXXFLAGS_MEDIA += ['-Werror', '-pipe', '-fexceptions', '--std=c++11']
        ctx.env.CXXFLAGS_WEB += ['-Werror', '-pipe', '-fexceptions', '--std=c++11']
        ctx.env.LINKFLAGS_MEDIA += ['-framework', 'CoreFoundation', '-framework', 'SystemConfiguration']
        if(debug):
            ctx.env.CXXFLAGS_MEDIA += ['-g']
    else:
        ctx.fatal("Unsupported build platform {0}".format(os.sys.platform))

    #Arrange library paths and store in ctx.env.LibraryPath
    ohNetLibraries = find_ohnet_libs(ctx, ctx.options.ohNetLibraries, ohNetPlatform)

    ohNetGeneratedLibraries = find_ohnet_libs(ctx, ctx.options.ohNetGeneratedLibraries, ohNetPlatform)


    ctx.env.LIBPATH_MEDIA = [ohNetLibraries, ohNetGeneratedLibraries]
    if ctx.options.disableVlc == False:
        ctx.env.LIBPATH_VLC = [vlcLibraries]
    ctx.env.LIBPATH_WEB = [civetwebLibraries, jsonhandleLibraries]

    #Let user know about selected paths
    print 'INCLUDES_MEDIA: {0}'.format(ctx.env.INCLUDES_MEDIA)
    print 'LIBPATH_MEDIA: {0}'.format(ctx.env.LIBPATH_MEDIA)
    print 'INCLUDES_WEB: {0}'.format(ctx.env.INCLUDES_WEB)
    print 'LIBPATH_WEB: {0}'.format(ctx.env.LIBPATH_WEB)

    #Ensure those directories actually exist
    #ctx.find_file('.', ctx.env.INCLUDES_MEDIA)
    #ctx.find_file('.', ctx.env.LIBPATH_MEDIA)

    ctx.define('APPNAME', APPNAME)
    ctx.define('VERSION', VERSION)
    ctx.write_config_header('config.h')


def build(ctx):
    from waflib import Build
    ctx.post_mode = Build.POST_LAZY

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
            'Renderers/Dummy/Dummy.cpp',
            'Renderers/resources.csr'
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

    config_includes = []
    config_includes.extend(ctx.env.INCLUDES_MEDIA)
    config_includes.extend(ctx.env.INCLUDES_WEB)
    ctx.stlib(
        source      = [
            'Config/Config.cpp',
            'Config/resources.csr'
        ],
        includes    = config_includes,
        target      = 'config',
        use         = 'WEB'
        )

    uses.append('config')

    ctx.program(
        source      = [
            'Renderers/main.cpp'
            ],
        includes    = ctx.env.INCLUDES_MEDIA,
        target      = 'ohMediaPlayer',
        stlib       = ['ohNetCore', 'ohNetGeneratedDevices', 'ohNetDevices', 'TestFramework'],
        use         = uses,
        )

# vim: set filetype=python softtabstop=4 expandtab shiftwidth=4 tabstop=4:
