
#ifndef RUBY_NATIVE_SFCC_CIM_DATA_H
#define RUBY_NATIVE_SFCC_CIM_DATA_H

#include "sfcc.h"

void init_cim_data();

extern VALUE cSfccCimData;
VALUE Sfcc_wrap_cim_data(CIMCData *data);
/**
 * deallocates inner value in case of pointer to data
 * not the CIMCData object itself
 */
void Sfcc_clear_cim_data(CIMCData *data);

#endif
