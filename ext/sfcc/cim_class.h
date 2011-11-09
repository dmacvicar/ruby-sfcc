
#ifndef RUBY_NATIVE_SFCC_CIM_CLASS_H
#define RUBY_NATIVE_SFCC_CIM_CLASS_H

#include "sfcc.h"

void init_cim_class();

extern VALUE cSfccCimClass;
VALUE Sfcc_wrap_cim_class(CMPIConstClass *cimclass, VALUE client);

#endif
