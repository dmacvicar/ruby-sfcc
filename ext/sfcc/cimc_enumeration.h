
#ifndef RUBY_NATIVE_SFCC_CIMC_ENUMERATION_H
#define RUBY_NATIVE_SFCC_CIMC_ENUMERATION_H

#include "sfcc.h"

void init_cimc_enumeration();

extern VALUE cSfccCimcEnumeration;
VALUE Sfcc_wrap_cimc_enumeration(CIMCEnumeration *enm);

#endif
