
#ifndef RUBY_NATIVE_SFCC_CIM_ENUMERATION_H
#define RUBY_NATIVE_SFCC_CIM_ENUMERATION_H

#include "sfcc.h"

void init_cim_enumeration();

extern VALUE cSfccCimEnumeration;
VALUE Sfcc_wrap_cim_enumeration(CMPIEnumeration *enm, VALUE client);

#endif
