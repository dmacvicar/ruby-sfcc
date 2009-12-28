
#ifndef RUBY_NATIVE_SFCC_CIMC_STRING_H
#define RUBY_NATIVE_SFCC_CIMC_STRING_H

#include "sfcc.h"

void init_cimc_string();

extern VALUE cSfccCimcString;
VALUE Sfcc_wrap_cimc_string(CIMCString *string);

#endif
