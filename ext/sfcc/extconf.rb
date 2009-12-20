require 'mkmf'
have_library('cimcclient', 'NewCIMCEnv')
find_header 'cmci.h', '/usr/include/CimClientLib'
create_makefile('sfcc')

