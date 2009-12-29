require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

have_library('cmpisfcc', 'NewCIMCEnv')
#find_header 'cimc.h', '/usr/include/cimc'
find_header 'cmci.h', '/usr/include/CimClientLib'
create_makefile('sfcc')

