require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

raise "Cannot find NewCIMCEnv() in libcmpisfcc" unless have_library('cmpisfcc', 'NewCIMCEnv')
#find_header 'cimc.h', '/usr/include/cimc'
find_header 'cmci.h', '/usr/include/CimClientLib'
create_makefile('sfcc')

