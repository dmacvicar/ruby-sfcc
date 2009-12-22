
#ifndef RUBY_NATIVE_SFCC_CIMC_CLASS_H
#define RUBY_NATIVE_SFCC_CIMC_CLASS_H

#include "sfcc.h"

void init_cimc_class();

extern VALUE cSfccCimcClass;
VALUE Sfcc_wrap_cimc_class(CIMCClass *cimclass);

#endif
