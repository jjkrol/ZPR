CCFLAGS = '-Wall -pedantic `pkg-config --cflags gtkmm-3.0`'
LINKFLAGS = ' `pkg-config --libs gtkmm-3.0`  -lboost_filesystem -lboost_system -lboost_thread -ljpeg -lsqlite3'

main = [ 'main.cpp' ]
core = [
    'src/directory.cpp',
    'src/photo.cpp',
    'src/disk.cpp',
    'src/core.cpp',
    'src/configurationManager.cpp',
    'src/photoCache.cpp',
    ]
asynchronous = [
    'src/asynchronous.cpp',
    'src/messageQueue.cpp',
    'src/ticket.cpp'
]
gui = [
    'src/gui.cpp',
    'src/dialogs.cpp',
    'src/window.cpp',
    'src/libraryView.cpp',
    'src/editView.cpp'
]
database = [
    'src/dbConnector.cpp',
    'src/hashFunctions.cpp'
]
tests = [
  'test/unit.cpp',
]
demo = [
  'test/async_test.cpp'
]
plugins = [
  'src/plugins/pluginManager.cpp',
  'src/plugins/samplePlugin.cpp',
  'src/plugins/sampleEffect.cpp',
  'src/plugins/desaturatePlugin.cpp',
  'src/plugins/desaturateEffect.cpp',
]
app = core + gui + database + asynchronous + plugins

env = Environment(CCFLAGS = CCFLAGS, LINKFLAGS = LINKFLAGS)
env.Append(LIBS=File('/usr/lib/libboost_unit_test_framework.a'))
env.Program(
  target = 'imagine',
  source = app + main
    )

program = env.Program('unit_test', \
  source = app + tests
    )
test_alias = Alias('unit_test', [program], program[0].path)
AlwaysBuild(test_alias)

async = env.Program('async', \
  source = asynchronous + demo
 )
