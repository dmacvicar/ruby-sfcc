require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

have_library('cmpisfcc', 'NewCIMCEnv')
find_header 'cmci.h', '/usr/include/CimClientLib'
create_makefile('sfcc')

