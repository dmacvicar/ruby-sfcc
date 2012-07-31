
#ifndef RUBY_NATIVE_SFCC_CIM_FLAGS_H
#define RUBY_NATIVE_SFCC_CIM_FLAGS_H

#include "sfcc.h"

void init_cim_flags();

extern VALUE cSfccCimFlags;
VALUE Sfcc_wrap_cim_flag(unsigned int flag);

#endif
