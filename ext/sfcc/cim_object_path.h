
#ifndef RUBY_NATIVE_SFCC_CIM_OBJECT_PATH_H
#define RUBY_NATIVE_SFCC_CIM_OBJECT_PATH_H

#include "sfcc.h"

void init_cim_object_path();

extern VALUE cSfccCimObjectPath;
VALUE Sfcc_wrap_cim_object_path(CMPIObjectPath *object_path, VALUE client);

#endif
