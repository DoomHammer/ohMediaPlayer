import sys
import os

from waflib.Node import Node
from waftools.filetasks import (
    find_resource_or_fail)

import os.path, sys
sys.path[0:0] = [os.path.join('dependencies', 'AnyPlatform', 'ohWafHelpers')]

from filetasks import gather_files, build_tree, copy_task
from utilfuncs import invoke_test, guess_dest_platform, configure_toolchain, guess_ohnet_location

APPNAME = 'ohMediaPlayer'
VERSION = '0.1.0'

sys.path.append('waftools')

def find_rel_or_abs(ctx, node):
    ret = ctx.path.find_node(node)
    if ret is None:
        ret = ctx.root.find_node(node)
    return ret

def options(opt):
    opt.load('compiler_cxx')
    opt.load('compiler_c')
    opt.add_option('--ohnet-include-dir', action='store', default='../ohNet/Build/Include', help='Path to root of ohNet header files')
    opt.add_option('--ohnet-lib-dir', action='store', default='../ohNet/Build/Obj', help='Path to root of ohNet library binaries')
    opt.add_option('--ohnet', action='store', default=None)
    opt.add_option('--vlcHeaders', action='store', default='/usr/include', help='Path to root of vlc header files')
    opt.add_option('--vlcLibraries', action='store', default='/usr/lib', help='Path to root of vlc library binaries')
    opt.add_option('--disableVlc', action='store_true', default=False, help='Should VLC support be built')
    opt.add_option('--civetwebHeaders', action='store', default='../civetweb/include', help='Path to root of civetweb header files')
    opt.add_option('--civetwebLibraries', action='store', default='../civetweb', help='Path to root of civetweb library binaries')
    opt.add_option('--jsonhandleHeaders', action='store', default='../jsonhandle/src', help='Path to root of jsonhandle header files')
    opt.add_option('--jsonhandleLibraries', action='store', default='../jsonhandle/lib', help='Path to root of jsonhandle library binaries')
    opt.add_option('--debug', action='store_const', const='Debug', default='Release', dest='debugmode', help='Generate and use binaries with debugging support')
    opt.add_option('--release', action='store_const', const='Release', default='Release', dest='debugmode', help='Generate and use binaries without debugging support')
    opt.add_option('--dest-platform', action='store', default=None)
    opt.add_option('--cross', action='store', default=None)

def configure(conf):

    if conf.options.dest_platform is None:
        try:
            conf.options.dest_platform = guess_dest_platform()
        except KeyError:
            conf.fatal('Specify --dest-platform')

    configure_toolchain(conf)
    guess_ohnet_location(conf)

    conf.load('csr2h', tooldir='./waftools')

    hpath = [
        '.',
        conf.path.find_node('.').abspath()
        ]
    conf.env.INCLUDES_MEDIA = hpath

    vlcHeaders = find_rel_or_abs(conf, conf.options.vlcHeaders)
    if not vlcHeaders:
        print 'VLC headers not found.'
        conf.options.disableVlc = True
    conf.env.DISABLEVLC = conf.options.disableVlc
    if conf.options.disableVlc == False:
        vlcHeaders = vlcHeaders.abspath()
        print 'VLC headers: ', vlcHeaders
        conf.env.INCLUDES_VLC = [vlcHeaders]
        conf.check(header_name='vlc/vlc.h', use=['VLC'])
        print 'Using VLC headers from %s' % vlcHeaders

    civetwebHeaders = find_rel_or_abs(conf, os.path.join(conf.options.civetwebHeaders))
    civetwebHeaders = civetwebHeaders.abspath()

    jsonhandleHeaders = find_rel_or_abs(conf, os.path.join(conf.options.jsonhandleHeaders))
    jsonhandleHeaders = jsonhandleHeaders.abspath()

    conf.env.INCLUDES_WEB = [civetwebHeaders, jsonhandleHeaders]

    vlcLibraries = None

    civetwebLibraries = find_rel_or_abs(conf, os.path.join(conf.options.civetwebLibraries))
    civetwebLibraries = civetwebLibraries.abspath()

    jsonhandleLibraries = find_rel_or_abs(conf, os.path.join(conf.options.jsonhandleLibraries))
    jsonhandleLibraries = jsonhandleLibraries.abspath()

    conf.env.LIB_WEB = ['jsonhandle', 'civetweb', 'pthread', 'dl']
    conf.env.LIBPATH_WEB = [civetwebLibraries, jsonhandleLibraries]

    if sys.platform == 'linux2':
        if conf.options.disableVlc == False:
            vlcLibraries = find_rel_or_abs(conf, conf.options.vlcLibraries)
            vlcLibraries = vlcLibraries.abspath()
            conf.env.LIB_VLC = ['vlc', 'vlccore']
            conf.env.LIBPATH_VLC = [vlcLibraries]

        conf.env.LIB_MEDIA = ['pthread']
    else:
        conf.fatal("Unsupported build platform {0}".format(os.sys.platform))

    #Let user know about selected paths
    print 'INCLUDES_MEDIA: {0}'.format(conf.env.INCLUDES_MEDIA)
    print 'LIBPATH_MEDIA: {0}'.format(conf.env.LIBPATH_MEDIA)
    print 'INCLUDES_WEB: {0}'.format(conf.env.INCLUDES_WEB)
    print 'LIBPATH_WEB: {0}'.format(conf.env.LIBPATH_WEB)

    conf.define('APPNAME', APPNAME)
    conf.define('VERSION', VERSION)
    conf.write_config_header('config.h')


class GeneratedFile(object):
    def __init__(self, xml, domain, type, version, target):
        self.xml = xml
        self.domain = domain
        self.type = type
        self.version = version
        self.target = target

upnp_services = [
        GeneratedFile('OpenHome/Av/ServiceXml/OpenHome/Product1.xml', 'av.openhome.org', 'Product', '1', 'AvOpenhomeOrgProduct1'),
        GeneratedFile('OpenHome/Av/ServiceXml/OpenHome/Volume1.xml', 'av.openhome.org', 'Volume', '1', 'AvOpenhomeOrgVolume1'),
        GeneratedFile('OpenHome/Av/ServiceXml/OpenHome/Info1.xml', 'av.openhome.org', 'Info', '1', 'AvOpenhomeOrgInfo1'),
        GeneratedFile('OpenHome/Av/ServiceXml/OpenHome/Playlist1.xml', 'av.openhome.org', 'Playlist', '1', 'AvOpenhomeOrgPlaylist1'),
        GeneratedFile('OpenHome/Av/ServiceXml/OpenHome/Time1.xml', 'av.openhome.org', 'Time', '1', 'AvOpenhomeOrgTime1'),
    ]


def build(bld):

    # Generated provider base classes
    t4templatedir = bld.env['T4_TEMPLATE_PATH']
    text_transform_exe_node = find_resource_or_fail(bld, bld.root, os.path.join(bld.env['TEXT_TRANSFORM_PATH'], 'TextTransform.exe'))
    for service in upnp_services:
        for t4Template, prefix, ext, args in [
                ('DvUpnpCppCoreHeader.tt', 'Dv', '.h', '-a buffer:1'),
                ('DvUpnpCppCoreSource.tt', 'Dv', '.cpp', '')
                ]:
            t4_template_node = find_resource_or_fail(bld, bld.root, os.path.join(t4templatedir, t4Template))
            tgt = bld.path.find_or_declare(os.path.join('Generated', prefix + service.target + ext))
            bld(
                rule="mono " + text_transform_exe_node.abspath() + " -o " + tgt.abspath() + " " + t4_template_node.abspath() + " -a xml:../" + service.xml + " -a domain:" + service.domain + " -a type:" + service.type + " -a version:" + service.version + " " + args,
                source=[text_transform_exe_node, t4_template_node, service.xml],
                target=tgt
                )
    bld.add_group()

    bld.install_files('${PREFIX}/include', [
            'OpenHome/Store.h',
        ], relative_trick=True)

    bld.install_files('${PREFIX}/share/Tests/TestStore', [
            'OpenHome/Tests/TestStore/defaults0.txt',
            'OpenHome/Tests/TestStore/defaultsString0.txt',
        ], relative_trick=False)

    bld.stlib(
        source = [
            'OpenHome/Store.cpp',
            'OpenHome/MurmurHash3.cpp'
        ],
        target = 'ohPersist',
        use    = ['OHNET', 'MEDIA'],
        includes = bld.env.INCLUDES_MEDIA
        )

    bld.program(
        source      = [
            'OpenHome/Tests/TestStore.cpp'
        ],
        includes    = bld.env.INCLUDES_MEDIA,
        target      = 'TestStore',
        install_path= '${PREFIX}/bin/Tests',
        stlib       = ['ohNetCore', 'TestFramework'],
        use         = ['OHNET', 'MEDIA', 'ohPersist']
        )

    bld.stlib(
        source = [
            'OpenHome/Media/Product.cpp',
            'OpenHome/Media/Info.cpp',
            'OpenHome/Media/Time.cpp',
            'OpenHome/Media/Volume.cpp',
            'OpenHome/Media/Playlist.cpp',
            'OpenHome/Media/Player.cpp',
            'OpenHome/Media/Standard.cpp',
            'OpenHome/Media/Source.cpp',
            'OpenHome/Media/SourcePlaylist.cpp',
            'Generated/DvAvOpenhomeOrgProduct1.cpp',
            'Generated/DvAvOpenhomeOrgVolume1.cpp',
            'Generated/DvAvOpenhomeOrgInfo1.cpp',
            'Generated/DvAvOpenhomeOrgPlaylist1.cpp',
            'Generated/DvAvOpenhomeOrgTime1.cpp'
        ],
        target = 'ohMedia',
        use    = ['OHNET', 'MEDIA', 'ohPersist'],
        includes = bld.env.INCLUDES_MEDIA
        )

    bld.stlib(
        source      = [
            'Renderers/Dummy/Dummy.cpp',
            'Renderers/resources.csr'
            ],
        includes    = bld.env.INCLUDES_MEDIA,
        target      = 'rendererDummy',
        use         = 'OHNET'
        )

    uses = ['OHNET', 'ohMedia', 'rendererDummy']

    if bld.env.DISABLEVLC == False:
        bld.stlib(
            source      = [
                'Renderers/Vlc/Vlc.cpp'
                ],
            includes    = bld.env.INCLUDES_MEDIA,
            target      = 'rendererVlc',
            use         = ['OHNET', 'VLC']
            )

        uses.append('rendererVlc')

    config_includes = []
    config_includes.extend(bld.env.INCLUDES_MEDIA)
    config_includes.extend(bld.env.INCLUDES_WEB)
    bld.stlib(
        source      = [
            'Config/Config.cpp',
            'Config/resources.csr'
        ],
        includes    = config_includes,
        target      = 'config',
        use         = ['OHNET', 'WEB']
        )

    uses.append('config')

    bld.program(
        source      = [
            'Renderers/main.cpp'
            ],
        includes    = bld.env.INCLUDES_MEDIA,
        target      = 'ohmediaplayer',
        stlib       = ['ohNetCore', 'ohNetDevices', 'TestFramework'],
        linkflags   = ['-lrt'],
        use         = uses,
        )

# vim: set filetype=python softtabstop=4 expandtab shiftwidth=4 tabstop=4:
