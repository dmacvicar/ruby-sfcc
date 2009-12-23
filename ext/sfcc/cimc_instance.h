
#ifndef RUBY_NATIVE_SFCC_CIMC_INSTANCE_H
#define RUBY_NATIVE_SFCC_CIMC_INSTANCE_H

#include "sfcc.h"

void init_cimc_instance();

extern VALUE cSfccCimcInstance;
VALUE Sfcc_wrap_cimc_instance(CIMCInstance *instance);

#endif
