
#ifndef RUBY_NATIVE_SFCC_CIMC_STRING_H
#define RUBY_NATIVE_SFCC_CIMC_STRING_H

#include "sfcc.h"

void init_cim_string();

extern VALUE cSfccCimString;
VALUE Sfcc_wrap_cim_string(CIMCString *string);

#endif
