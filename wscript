"""
Waf script used to build:
 * core engine library
 * game library
 * game launcher application
 * fonttool application
 * generate protocol buffers C++/Python classes

Usage:
 waf configure build
 waf configure -t debug --test=1 build
 waf configure -t release --fonttool=1
 CXX=clang waf configure build
 ...

Notes:
 * supported platforms: linux, windows
 * platform macros: __linux__, _WIN32
 * compiler macros: __GNUC__, __clang__, _MSC_VER

Compiled on:
 * gcc 4.7.3
 * clang 3.2 (doesn't support OpenMP)
 * msvc 12.0 (OpenMP not tested)

"""

from waflib.Logs import log
from waflib import Options, Logs
from sys import platform
import os


top = "."
out = "build"


class BuildType:
    RELEASE = 'release'
    PROFILE = 'profile'
    DEBUG = 'debug'


def options(ctx):
    """Define build options
    """
    ctx.load('compiler_cxx qt4')
    ctx.add_option('-t', '--type', action='store', default='normal', help='Build type debug/normal/release/profile')
    ctx.add_option('--test', action='store', default='0', help='Build tests 0/1')
    ctx.add_option('--fonttool', action='store', default='0', help='Build fonttool 0/1')


def configure(ctx):
    """Configure the build
    Find required libraries
    Configure compile flags for 'core' lib (CORE), ???
    """
    ctx.load('compiler_cxx qt4')

    type = BuildType.DEBUG

    if ctx.options.type == 'release':
        type = BuildType.RELEASE
    elif ctx.options.type == 'profile':
        type = BuildType.PROFILE
    elif ctx.options.type == 'debug':
        type = BuildType.DEBUG


    if str_to_bool(ctx.options.test):
        ctx.env.append_unique('MY_BUILD_TESTS', ['1'])

    if ctx.options.fonttool != '0':
        ctx.env.append_unique('MY_BUILD_FONTTOOL', ['1'])

    if is_linux():
        get_linux_params(type, ctx)
        ctx.env.append_unique('MY_DEFINES', ['_GLIBCXX_USE_NANOSLEEP'])
        check_required_linux_libs(ctx);
    elif is_windows():
        get_windows_params(type, ctx)
        check_required_windows_libs(ctx)
    else:
         ctx.fatal('Unsupported platform: ' + platform)

    # summary
    print_summary(ctx)


def conf(ctx):
    """Alias for the configure command
    """
    Options.commands = ['configure'] + Options.commands


def build(ctx):
    build_core_lib(ctx)
    build_game_lib(ctx)
    #build_starter(ctx)
    build_editor(ctx)

    if 'MY_BUILD_FONTTOOL' in ctx.env:
        build_fonttool(ctx)

    if 'MY_BUILD_TESTS' in ctx.env:
        build_tests(ctx)


#------------------------------------------------------------------------------
#
# Platform specific flags (compiler, linker, ...)
#
#------------------------------------------------------------------------------


def get_linux_params(type, ctx):
    """Add linux compiler/linker/... flags to the context

    :type ctx: waflib.Configure.ConfigurationContext
    :param ctx: configuration context
    """
    ctx.env.append_unique('CXXFLAGS', ['-std=c++11', '-Wall', '-Wno-invalid-offsetof'])
    ctx.env.append_value('CXXFLAGS', ['-I', '/usr/local/include/bullet'])

    if type == BuildType.RELEASE:
        ctx.env.append_unique('CXXFLAGS', ['-O2', '-g'])
        ctx.env.append_unique('DEFINES', ['NDEBUG'])
    elif type == BuildType.PROFILE:
        ctx.env.append_unique('CXXFLAGS', ['NDEBUG', '-O2', '-g', '-pg'])
    elif type == BuildType.DEBUG:
        ctx.env.append_unique('CXXFLAGS', ['-O0', '-g'])
    else:
        ctx.fatal('Unsupported build type: ' + type)


def get_windows_params(type, ctx):
    """Add windows compiler/linker/... flags to the context

    :type ctx: waflib.Configure.ConfigurationContext
    :param ctx: configuration context
    """

    thirdparty = os.environ.get("THIRDPARTY")

    if thirdparty == None:
        ctx.fatal("Missing environment variable THIRDPARTY")

    # potrebne kvoli M_PI a dalsim konstantam z matth.h
    ctx.env.append_unique('DEFINES', ['_USE_MATH_DEFINES'])
    ctx.env.append_unique('CXXFLAGS', ['/EHsc', '/MD', '/FS'])
    ctx.env.append_value('CXXFLAGS', ['/I', thirdparty + '/include'])
    ctx.env.append_value('LINKFLAGS', ['/NODEFAULTLIB:libcmt.lib', '/ignore:4204'])
    ctx.env.append_value('LIBPATH', [thirdparty + '/lib'])


    if type == BuildType.DEBUG:
        ctx.env.append_unique('CXXFLAGS', ['/Zi', '/Od'])
        ctx.env.append_unique('LINKFLAGS', ['/debug'])
    else:
        ctx.env.append_unique('CXXFLAGS', ['/O2', '/Z7'])


#------------------------------------------------------------------------------
#
# Locate and check required libraries
#
#------------------------------------------------------------------------------


def check_required_linux_libs(ctx):
    """Locate libraries required for build and test them (linux version)

    :type ctx: waflib.Configure.ConfigurationContext
    :param ctx: configuration context
    """
    ctx.check_cxx(uselib_store='ogg', header_name='ogg/ogg.h', lib=['ogg'])
    ctx.check_cxx(uselib_store='vorbisfile', header_name='vorbis/vorbisfile.h', lib=['vorbisfile'])
    ctx.check_cxx(uselib_store='GLEW', header_name='GL/glew.h', lib=['GLEW', 'GL'])
    ctx.check_cxx(uselib_store='png', header_name='libpng/png.h', lib=['png'])
    ctx.check_cxx(uselib_store='SDL', header_name='SDL/SDL.h', lib=['SDL'])
    ctx.check_cxx(uselib_store='pthread', header_name='pthread.h', lib=['pthread'])
    ctx.check_cxx(uselib_store='bullet', header_name='btBulletCollisionCommon.h', lib=['LinearMath', 'BulletCollision', 'BulletDynamics', 'BulletSoftBody'])

    if 'MY_BUILD_TESTS' in ctx.env:
        ctx.check_cxx(uselib_store='gtest', header_name='gtest/gtest.h', lib=['gtest_main', 'gtest'], use=['pthread'])


def check_required_windows_libs(ctx):
    """Locate libraries required for build and test them (windows version)

    :type ctx: waflib.Configure.ConfigurationContext
    :param ctx: configuration context
    """
    ctx.check_cxx(lib=['libogg'], uselib_store='ogg', header_name='ogg/ogg.h')
    ctx.check_cxx(lib=['libvorbisfile', 'libvorbis'], uselib_store='vorbisfile', header_name='vorbis/vorbisfile.h')
    ctx.check_cxx(lib=['glew32', 'OpenGL32'], uselib_store='GLEW', header_name='GL/glew.h')
    ctx.check_cxx(lib=['SDL'], uselib_store='SDL', header_name='SDL/SDL.h')
    ctx.check_cxx(lib=['libpng16'], uselib_store='png', header_name='png.h')
    ctx.check_cxx(uselib_store='bullet', header_name='btBulletCollisionCommon.h', lib=['LinearMath', 'BulletCollision', 'BulletDynamics', 'BulletSoftBody'])


#------------------------------------------------------------------------------
#
# Build section (build targets)
#
#------------------------------------------------------------------------------


def build_core_lib(ctx):
    """build Core engine as static lib"""
    #filter = 'src/**/crate.cpp'
    ctx.stlib(
      name='core',
      target = 'core',
      source=ctx.path.ant_glob('src/libcore/**/crate.cpp'),
      #source=ctx.path.ant_glob('src/libcore/**/*.cpp', excl=filter),
      export_includes=['src'],
      use=['ogg', 'vorbisfile', 'GLEW', 'png', 'SDL']
    )


def build_game_lib(ctx):
    """build Game lib as static lib"""
    #filter = 'src/libgame/**/crate.cpp'
    ctx.stlib(
      name='game',
      target = 'game',
      source=ctx.path.ant_glob('src/libgame/**/crate.cpp'),
      #source=ctx.path.ant_glob('src/libgame/**/*.cpp', excl=filter),
      includes=['src/libcore', 'src/libgame'],
      use=['core']
    )


def build_starter(ctx):
    ctx.program(
      name='run',
      target = 'run',
      source=ctx.path.ant_glob('src/runner/**/*.cpp'),
      includes=['src/libcore', 'src/libgame'],
      use=['game', 'core']
    )


def build_editor(ctx):
    ctx.program(
      name='edit',
      target='edit',
      features='cxx cxxprogram qt4',
      source=ctx.path.ant_glob('src/editor/**/crate.cpp') + ctx.path.ant_glob('src/editor/**/*.ui'),
      includes=['src/libcore', 'src/libgame', 'build/src/editor/ui'],
      use=['game', 'core', 'QTCORE', 'QTGUI', 'QTOPENGL']
    )


def build_fonttool(ctx):
    ctx.program(
      name='fonttool',
      target='mmfonttool',
      features='cxx cxxprogram',
      source = ctx.path.ant_glob('tools/fonttool/src/**/*.cpp'),
      cxxflags=['-fopenmp'],
      linkflags=['-Wl,-lstdc++', '-fopenmp'],
      includes=['src'],
      use=['core']
    )


def build_tests(ctx):
    ctx.program(
      name='test',
      target='test',
      features='test',
      source=ctx.path.ant_glob('src/tests/**/*.cpp'),
      includes=['src'],
      use=['core', 'gtest', 'pthread']
    )
    from waflib.Tools import waf_unit_test
    ctx.add_post_fun(waf_unit_test.summary)


#------------------------------------------------------------------------------
#
# Utilities
#
#------------------------------------------------------------------------------


def str_to_bool(value):
    """Convert string value to bool

    :type value: str
    :param value: '1' means True, other values means False
    """
    return True if value == '1' else False

def print_summary(ctx):
    """Print build summary (from values stored in ctx.env)

    :type ctx: waflib.Configure.ConfigurationContext
    :param ctx: configuration context
    """
    Logs.pprint('BLUE', 'Platform : ' + platform)
    Logs.pprint('BLUE', 'Compiler : ' + str(ctx.env.CXX))
    Logs.pprint('BLUE', 'Monolith : ' + str(ctx.env.MONOLITH))
    Logs.pprint('BLUE', 'CXXFLAGS : ' + str(ctx.env.CXXFLAGS))
    Logs.pprint('BLUE', 'LINKFLAGS: ' + str(ctx.env.LINKFLAGS))

    modules = ['core', 'game']

    if 'MY_BUILD_TESTS' in ctx.env:
        modules.append('test')

    Logs.pprint('BLUE', 'Modules  : ' + str(modules))


def is_linux():
    return platform.startswith('linux')


def is_windows():
    return platform.startswith('win32')
