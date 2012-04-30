#ifndef CIM_DATA_H_
#define CIM_DATA_H_

#include "sfcc.h"

void init_cim_data();

extern VALUE cSfccCimData;
VALUE Sfcc_wrap_cim_data(CIMCData *cimdata);

#endif /* ----- CIM_DATA_H_ ----- */

