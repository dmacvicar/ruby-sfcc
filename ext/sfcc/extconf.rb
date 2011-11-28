require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

unless have_library('cmpisfcc', 'NewCIMCEnv')
  STDERR.puts "Cannot find NewCIMCEnv() in libcmpisfcc"
  STDERR.puts "Is sblim-sfcc-devel installed ?"
end

find_header 'cimc.h', '/usr/include/cimc'
find_header 'cmcimacs.h', '/usr/include/CimClientLib'
create_makefile('sfcc')
