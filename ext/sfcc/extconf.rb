require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

have_library('cmpisfcc', 'NewCIMCEnv')
find_header 'cimc.h', '/usr/include/cimc'
create_makefile('sfcc')

