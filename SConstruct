CCFLAGS = '`pkg-config --cflags gtk+-2.0`'
LINKFLAGS = '`pkg-config --libs gtk+-2.0`'

env = Environment(CCFLAGS = CCFLAGS, LINKFLAGS = LINKFLAGS)
env.Program(target = 'gtk_test', source = ["gtk_test.cpp"])
