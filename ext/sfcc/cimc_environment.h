
#ifndef RUBY_NATIVE_SFCC_CIMC_ENV_H
#define RUBY_NATIVE_SFCC_CIMC_ENV_H

#include "sfcc.h"

void init_cimc_environment();

extern VALUE cSfccCimcEnvironment;
VALUE Sfcc_wrap_cimc_environment(CIMCEnv *env);

#endif
