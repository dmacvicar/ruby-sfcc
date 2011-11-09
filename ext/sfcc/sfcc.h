
#ifndef RUBY_SFCC_NATIVE
#define RUBY_SFCC_NATIVE

#include <assert.h>

/*
#include <cimc/cimc.h>
#include <cimc/cimcdt.h>
#include <cimc/cimcft.h>
*/
#include <CimClientLib/cmci.h>
#include <CimClientLib/cmcidt.h>
#include <CimClientLib/cmcift.h>
#include <CimClientLib/native.h>
#include <CimClientLib/cmcimacs.h>

#include "ruby.h"

#include <ruby.h>
#ifdef HAVE_RUBY_ST_H
# include <ruby/st.h>
#else
# include <st.h>
#endif

#include <unistd.h>
#include <stdlib.h>

extern const char *to_charptr(VALUE v);
extern VALUE mSfcc;
extern VALUE mSfccCim;

/*
#define SFCC_DEC_REFCOUNT(x) \
  do { \
    int refc = ((CIMCObject*)x)->refCount; \
    printf("dec X: %d rf: %d\n", (int)x, refc); \
    --((CIMCObject*)x)->refCount;          \
    if (refc <= 0) \
      x->ft->release(x); \
    free(x); \
  } while (0);
*/

#define SFCC_DEC_REFCOUNT(x) \
  x->ft->release(x);

/*
#define SFCC_INC_REFCOUNT(x) \
  do { \
    int refc = ((CIMCObject*)x)->refCount; \
    printf("inc X: %d rf: %d\n", (int) x, refc); \
    ++((CIMCObject*)x)->refCount;          \
  } while (0);
*/

#define SFCC_INC_REFCOUNT(x) (void)(0);

#define CIMSTR_2_RUBYSTR(x) (x ? (x->ft->getCharPtr(x, NULL) ? rb_str_new2(x->ft->getCharPtr(x, NULL)) : Qnil) : Qnil)

/**
 * raises a ruby exception if the status is an error
 * whenever possible, adds the custom message if not null
 */
void sfcc_rb_raise_if_error(CMPIStatus status, const char *msg, ...);

/**
 * allocates a string array where each string points to the
 * str pointer of the value
 *
 * you need to deallocate the array yourself. But don't deallocate
 * the elements.
 */
inline const char ** sfcc_value_array_to_string_array(VALUE array);

/**
 * converts a ruby hash to a CIM args object
 */
inline CMPIArgs* sfcc_hash_to_cimargs(VALUE hash);

/**
 * converts a CIM args object to a hash
 */
inline VALUE sfcc_cimargs_to_hash(CMPIArgs *args, VALUE client);

/**
 * converts CMPIData to ruby VALUE
 */
inline VALUE sfcc_cimdata_to_value(CMPIData data, VALUE client);

/**
 * convert ruby VALUE to CMPIData
 */
inline CMPIData sfcc_value_to_cimdata(VALUE value);

struct mark_struct {
  void *cmpi_object;
  VALUE ruby_value;
};
#endif
