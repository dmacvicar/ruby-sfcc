
#ifndef RUBY_NATIVE_SFCC_CIMC_CLIENT_H
#define RUBY_NATIVE_SFCC_CIMC_CLIENT_H

#include "sfcc.h"

void init_cimc_client();

extern VALUE cSfccCimcClient;
VALUE Sfcc_wrap_cimc_client(CIMCClient *client);

#endif
