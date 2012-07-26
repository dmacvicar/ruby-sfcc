
#ifndef RUBY_SFCC_NATIVE
#define RUBY_SFCC_NATIVE

#include <assert.h>

#include <cimc/cimc.h>
#include <cimc/cimcdt.h>
#include <cimc/cimcft.h>
        
#include "ruby.h"

#include <ruby.h>
#ifdef HAVE_RUBY_ST_H
# include <ruby/st.h>
#else
# include <st.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

extern const char *to_charptr(VALUE v);
extern VALUE mSfcc;
extern VALUE mSfccCim;
extern CIMCEnv *cimcEnv;
extern char *cimcEnvType;

#define CIMSTR_2_RUBYSTR(x) (x ? (x->ft->getCharPtr(x, NULL) ? rb_str_new2(x->ft->getCharPtr(x, NULL)) : Qnil) : Qnil)
#define CIMCRelease(o)         ((o)->ft->release((o)))
#define CIMCClone(o, rc)       ((o)->ft->clone((o), (rc)))
#define CIMCRelease(o)         ((o)->ft->release((o)))
#define CIMCGetCharsPtr(st, rc)      ((st)->ft->getCharPtr((st), (rc)))
#define CIMCGetArrayElementAt(a, n, rc) \
    ((a)->ft->getElementAt((a), (n), (rc)))

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
 * converts a CIMCArray to rbArray
 * makes a Sfcc::Cim::Data object (with deep copy) for every element of array
 */
inline VALUE sfcc_cimcarray_to_rubyarray(CIMCArray *array, VALUE client, bool deep_copy);

/**
 * converts a ruby hash to a CIM args object
 * caller must free it manually by calling CIMCRelease(result)
 */
inline CIMCArgs* sfcc_hash_to_cimargs(VALUE hash);

/**
 * converts a CIM args object to a hash
 * @param deep_copy whether to make a deep copy of array's contents,
 * that will be stored in ruby hash object
 */
inline VALUE sfcc_cimargs_to_hash(CIMCArgs *args, VALUE client, bool deep_copy);

/**
 * converts CIMCData to ruby VALUE
 * @param deep_copy whether to clone inner value (in case of pointer to object)
 * or just assign it (shallow copy)
 */
inline VALUE sfcc_cimdata_to_value(CIMCData *data, VALUE client, bool deep_copy);

/**
 * convert ruby VALUE to CIMCData
 * does not make a deep copy in case of wrapped objects
 */
inline CIMCData sfcc_value_to_cimdata(VALUE value);

#endif
