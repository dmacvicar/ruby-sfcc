
#ifndef RUBY_NATIVE_SFCC_CIMC_OBJECT_PATH_H
#define RUBY_NATIVE_SFCC_CIMC_OBJECT_PATH_H

#include "sfcc.h"

void init_cimc_object_path();

extern VALUE cSfccCimcObjectPath;
VALUE Sfcc_wrap_cimc_object_path(CIMCObjectPath *object_path);

#endif
