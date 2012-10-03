# setting help prompt
Help("""
Type: 'scons imagine' or 'scons' to build the Imagine application,
      'scons tests' to build the Imagine unit tests,
      'scons tests cov=1' to build tests with coverage control.
NOTE: It is safer to use coverage.py script to perform coverage control tests!
""")

# configuring environment
CCFLAGS = '-Wall -pedantic `pkg-config --cflags gtkmm-3.0`'
LINKFLAGS = '`pkg-config --libs gtkmm-3.0` -lboost_filesystem -lboost_system -lboost_thread -lsqlite3'
env = Environment(CCFLAGS = CCFLAGS, LINKFLAGS = LINKFLAGS)

# setting additional parameters if code coverage tests are needed
if(int(ARGUMENTS.get('cov', 0))):
  env.Append(CCFLAGS = ' -fprofile-arcs -ftest-coverage')
  env.Append(LINKFLAGS = ' -fprofile-arcs')

# defining source files list
main = [ 'main.cpp' ]
core = [
  'core/directory.cpp',
  'core/photo.cpp',
  'core/disk.cpp',
  'core/core.cpp',
  'core/configurationManager.cpp',
  'core/photoCache.cpp'
]
async = [
  'core/asynchronous.cpp',
  'core/messageQueue.cpp',
  'core/ticket.cpp'
]
gui = [
  'gui/gui.cpp',
  'gui/dialogs.cpp',
  'gui/prompts.cpp',
  'gui/window.cpp',
  'gui/libraryView.cpp',
  'gui/editView.cpp'
]
db = [
  'db/dbConnector.cpp',
  'db/hashFunctions.cpp'
]
plugins = [
  'plugins/pluginManager.cpp',
  'plugins/samplePlugin.cpp',
  'plugins/desaturatePlugin.cpp'
]
effects = [
  'effects/sampleEffect.cpp',
  'effects/desaturateEffect.cpp'
]
tests = [
  'test/unit.cpp'
]

# defining application ingredients
app = core + gui + db + async + plugins + effects
imagine = env.Program(target = 'imagine', source = app + main)
Default(imagine)

# defining unit test ingredients
env.Program(target = 'tests', source = env.Object(app) + tests, \
            LIBS=File('/usr/lib/libboost_unit_test_framework.a'))
