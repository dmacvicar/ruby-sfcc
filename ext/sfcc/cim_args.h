
#ifndef RUBY_NATIVE_SFCC_CIM_ARGS_H
#define RUBY_NATIVE_SFCC_CIM_ARGS_H

#include "sfcc.h"

void init_cim_args();

extern VALUE cSfccCimArgs;
VALUE Sfcc_wrap_cim_args(CMPIArgs *args);

#endif
