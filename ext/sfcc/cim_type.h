
#ifndef RUBY_NATIVE_SFCC_CIM_TYPE_H
#define RUBY_NATIVE_SFCC_CIM_TYPE_H

#include "sfcc.h"

void init_cim_type();

extern VALUE cSfccCimType;
VALUE Sfcc_wrap_cim_type(CIMCType type);
char const * Sfcc_cim_type_to_s(CIMCType type);
CIMCType Sfcc_rb_type_to_i(VALUE type);

#endif
