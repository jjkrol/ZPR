# run:
# scons - for simple build
# scons test - for building tests

CCFLAGS = '-Wall -pedantic `pkg-config --cflags gtkmm-3.0`'
LINKFLAGS = '`pkg-config --libs gtkmm-3.0`'

env = Environment(CCFLAGS = CCFLAGS, LINKFLAGS = LINKFLAGS)
env.Append(LIBS=File('/usr/lib/libboost_unit_test_framework.a'))
env.Program(target = 'imgview', source = ["main.cpp"])
program = env.Program('test', source = ['unit.cpp', 'core.cpp'])
test_alias = Alias('test', [program], program[0].path)
AlwaysBuild(test_alias)
