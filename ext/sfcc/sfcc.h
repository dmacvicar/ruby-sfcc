
#ifndef RUBY_SFCC_NATIVE
#define RUBY_SFCC_NATIVE

#include <assert.h>

#include <cimc/cimc.h>
#include <cimc/cimcdt.h>
#include <CimClientLib/cmci.h>
#include <CimClientLib/native.h>
#include <CimClientLib/cmcimacs.h>

#include "ruby.h"
#include <unistd.h>
#include <stdlib.h>

extern VALUE mSfcc;
extern VALUE mSfccCimc;

/**
 * raises a ruby exception if the status is an error
 * whenever possible, adds the custom message if not null
 */
void sfcc_rb_raise_if_error(CIMCStatus status, const char *msg, ...);

/**
 * allocates a string array where each string points to the
 * str pointer of the value
 *
 * you need to deallocate the array yourself. But don't deallocate
 * the elements.
 */
inline char ** sfcc_value_array_to_string_array(VALUE array);

/**
 * converts CIMCData to ruby VALUE
 */
inline VALUE sfcc_cimcdata_to_value(CIMCData data);

#endif
