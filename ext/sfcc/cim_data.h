
#ifndef RUBY_NATIVE_SFCC_CIM_DATA_H
#define RUBY_NATIVE_SFCC_CIM_DATA_H

#include "sfcc.h"

void init_cim_data();

extern VALUE cSfccCimData;
VALUE Sfcc_wrap_cim_data(CIMCData *data);

#endif
