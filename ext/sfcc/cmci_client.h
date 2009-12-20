
#ifndef RUBY_NATIVE_SFCC_CMCI_CLIENT_H
#define RUBY_NATIVE_SFCC_CMCI_CLIENT_H

#include "sfcc.h"

void init_sfcc_cmci_client();

extern VALUE cSfccCmciClient;
VALUE Sfcc_wrap_cmci_client(VALUE klass, CMCIClient *client);

#endif
