
#ifndef RUBY_NATIVE_SFCC_CIM_CLIENT_H
#define RUBY_NATIVE_SFCC_CIM_CLIENT_H

#include "sfcc.h"

void init_cim_client();

extern VALUE cSfccCimClient;
VALUE Sfcc_wrap_cim_client(CMCIClient *client);

#endif
