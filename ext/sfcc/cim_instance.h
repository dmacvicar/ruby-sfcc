
#ifndef RUBY_NATIVE_SFCC_CIM_INSTANCE_H
#define RUBY_NATIVE_SFCC_CIM_INSTANCE_H

#include "sfcc.h"

void init_cim_instance();

extern VALUE cSfccCimInstance;
VALUE Sfcc_wrap_cim_instance(CMPIInstance *instance, VALUE client);

#endif
