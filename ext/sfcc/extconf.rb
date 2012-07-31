require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

unless have_library('cimcclient', 'NewCIMCEnv')
  STDERR.puts "Cannot find NewCIMCEnv() in libcimcclient"
  STDERR.puts "Is sblim-sfcc-devel installed ?"
  exit 1
end

find_header 'cimc.h', '/usr/include/cimc'
create_makefile('sfcc')
