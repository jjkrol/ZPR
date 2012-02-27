require 'rake/clean'

task :default => 'gtk_test'

CC = 'g++'
CFLAGS ='`pkg-config --libs gtk+-2.0`'   
LINKFLAGS = '`pkg-config --cflags gtk+-2.0`'
OFILES = FileList['*.cpp'].sub!(/\.cpp$/, '.o')
file 'gtk_test' => OFILES do |t|
  sh "#{CC} #{CFLAGS} #{OFILES} -o #{t.name}"
end

rule '.o' => ['.cpp'] do |t|
  sh "#{CC} #{LINKFLAGS} -c -o #{t.name} #{t.source}  "
end

CLEAN.include('*.o')
CLOBBER.include('gtk_test')
