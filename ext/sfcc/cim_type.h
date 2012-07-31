
#ifndef RUBY_NATIVE_SFCC_CIM_TYPE_H
#define RUBY_NATIVE_SFCC_CIM_TYPE_H

#include "sfcc.h"

void init_cim_type();

extern VALUE cSfccCimType;
VALUE Sfcc_wrap_cim_type(unsigned int type);

#endif
