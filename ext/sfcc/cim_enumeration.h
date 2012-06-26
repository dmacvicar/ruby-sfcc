
#ifndef RUBY_NATIVE_SFCC_CIM_ENUMERATION_H
#define RUBY_NATIVE_SFCC_CIM_ENUMERATION_H

#include "sfcc.h"

void init_cim_enumeration();

typedef struct {
  CIMCEnumeration *enm;
  VALUE client;
} rb_sfcc_enumeration;

extern VALUE cSfccCimEnumeration;
VALUE Sfcc_wrap_cim_enumeration(CIMCEnumeration *enm, VALUE client);

#endif
