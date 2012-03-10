# run:
# scons - for simple build
# scons test - for building tests

CCFLAGS = '-Wall -pedantic `pkg-config --cflags gtkmm-3.0`'
LINKFLAGS = '`pkg-config --libs gtkmm-3.0`  -lboost_filesystem -lboost_system -ljpeg'

env = Environment(CCFLAGS = CCFLAGS, LINKFLAGS = LINKFLAGS)
env.Append(LIBS=File('/usr/lib/libboost_unit_test_framework.a'))
env.Program(target = 'imgview', source = ["main.cpp"])
program = env.Program('unit_test', \
  source = [
    'test/unit.cpp', 
    'src/directory.cpp',
    'src/photo.cpp'
    ])
test_alias = Alias('unit_test', [program], program[0].path)
AlwaysBuild(test_alias)

env.Program(target = 'core',
   source = ["src/core.cpp", "src/directory.cpp", "src/photo.cpp" ])
