
#ifndef RUBY_NATIVE_SFCC_CIMC_ARGS_H
#define RUBY_NATIVE_SFCC_CIMC_ARGS_H

#include "sfcc.h"

void init_cimc_args();

extern VALUE cSfccCimcArgs;
VALUE Sfcc_wrap_cimc_args(CIMCArgs *args);

#endif
