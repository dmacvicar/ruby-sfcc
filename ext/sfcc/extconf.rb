require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

unless have_library('cmpisfcc', 'cmciConnect')
  STDERR.puts "Cannot find cmciConnect() in libcmpisfcc"
  STDERR.puts "Is sblim-sfcc-devel installed ?"
end

#find_header 'cimc.h', '/usr/include/cimc'
find_header 'cmci.h', '/usr/include/CimClientLib'
create_makefile('sfcc')

