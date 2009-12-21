require 'mkmf'
$CFLAGS = "#{ENV['CFLAGS']} -Wall -O3 "
have_library('cmpisfcc', 'NewCIMCEnv')
find_header 'cmci.h', '/usr/include/CimClientLib'
create_makefile('sfcc')

