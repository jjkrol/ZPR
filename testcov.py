# Python script for performing code coverage tests
# for Imagine application using scons, gcov and lcov

import os
import subprocess

subprocess.call(['rm', 'DB.sqlite'])
subprocess.call(['scons', '-c'])
subprocess.call(['scons', 'tests gcov=1'])
subprocess.call(['./tests'])

DIR = 'gcov'
if not os.path.exists(DIR):
  os.mkdir(DIR)
os.chdir(DIR)

subprocess.call(['lcov', '-b', '../', '-d', '../', '--capture', '-o', 'tests.info'])
subprocess.call(['lcov', '-a', 'tests.info', '-o', 'all.info'])
subprocess.call(['genhtml', 'all.info'])
subprocess.call(['rm', '*.gcno'])
subprocess.call(['rm', '*.gcda'])
