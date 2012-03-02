CCFLAGS = '-Wall -pedantic `pkg-config --cflags gtkmm-3.0`'
LINKFLAGS = '`pkg-config --libs gtkmm-3.0`'

env = Environment(CCFLAGS = CCFLAGS, LINKFLAGS = LINKFLAGS)
env.Program(target = 'imgview', source = ["main.cpp"])
