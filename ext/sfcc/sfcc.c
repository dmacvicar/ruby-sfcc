
#include <stdarg.h>
#include "sfcc.h"

#include "cimc_environment.h"
#include "cimc_object_path.h"
#include "cimc_enumeration.h"
#include "cimc_class.h"
#include "cimc_instance.h"
#include "cimc_client.h"

VALUE mSfcc;
VALUE mSfccCimc;

void Init_sfcc()
{
  mSfcc = rb_define_module("Sfcc");
  mSfccCimc= rb_define_module_under(mSfcc, "Cimc");

  init_cimc_environment();
  init_cimc_object_path();
  init_cimc_enumeration();
  init_cimc_class();
  init_cimc_instance();
  init_cimc_client();
}

static VALUE sfcc_status_exception(CIMCStatus status)
{
  switch (status.rc)
  {
  case CIMC_RC_ERR_FAILED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorFailed"));
  case CIMC_RC_ERR_ACCESS_DENIED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorAcessDenied"));
  case CIMC_RC_ERR_INVALID_NAMESPACE:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidNamespace"));
  case CIMC_RC_ERR_INVALID_PARAMETER:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidParameter"));
  case CIMC_RC_ERR_INVALID_CLASS:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidClass"));
  case CIMC_RC_ERR_NOT_FOUND:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNotFound"));
  case CIMC_RC_ERR_NOT_SUPPORTED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNotSupported"));
  case CIMC_RC_ERR_CLASS_HAS_CHILDREN:
    return rb_const_get(mSfccCimc, rb_intern("ErrorClassHasChildren"));
  case CIMC_RC_ERR_CLASS_HAS_INSTANCES:
    return rb_const_get(mSfccCimc, rb_intern("ErrorClassHasInstances"));
  case CIMC_RC_ERR_INVALID_SUPERCLASS:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidSuperClass"));
  case CIMC_RC_ERR_ALREADY_EXISTS:
    return rb_const_get(mSfccCimc, rb_intern("ErrorAlreadyExists"));
  case CIMC_RC_ERR_NO_SUCH_PROPERTY:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNoSuchProperty"));
  case CIMC_RC_ERR_TYPE_MISMATCH:
    return rb_const_get(mSfccCimc, rb_intern("ErrorTypeMismatch"));
  case CIMC_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorQueryLanguageNotSupported"));
  case CIMC_RC_ERR_INVALID_QUERY:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidQuery"));
  case CIMC_RC_ERR_METHOD_NOT_AVAILABLE:
    return rb_const_get(mSfccCimc, rb_intern("ErrorMethodNotAvailable"));
  case CIMC_RC_ERR_METHOD_NOT_FOUND:
    return rb_const_get(mSfccCimc, rb_intern("ErrorMethodNotFound"));
  case CIMC_RC_DO_NOT_UNLOAD:
    return rb_const_get(mSfccCimc, rb_intern("ErrorDoNotUnload"));
  case CIMC_RC_NEVER_UNLOAD:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNeverUnload"));
  case CIMC_RC_ERROR_SYSTEM:
    return rb_const_get(mSfccCimc, rb_intern("ErrorSystem"));
  case CIMC_RC_ERROR:
    return rb_const_get(mSfccCimc, rb_intern("ErrorRcError"));
  case CIMC_RC_OK:
  default:
    return Qnil;
  }
}

void sfcc_rb_raise_if_error(CIMCStatus status, const char *msg, ...)
{
  va_list arg_list;
  va_start(arg_list, msg);

  if (!status.rc)
    return;

  rb_raise(sfcc_status_exception(status), msg, arg_list);
}

char ** sfcc_value_array_to_string_array(VALUE array)
{
  char **ret;
  int i = 0;

  if ( !NIL_P(array) && RARRAY_LEN(array) > 0 ) {
    ret = (char**) malloc(RARRAY_LEN(array)*sizeof(char*));
    for (; i < RARRAY_LEN(array); ++i)
      ret[i] = StringValuePtr(*(RARRAY_PTR(array) + i));
  }
  else
    ret = NULL;

  return ret;
}

VALUE sfcc_cimcdata_to_value(CIMCData data)
{
  CIMCString *cimstr = NULL;
  VALUE rbval;
  CIMCStatus status;

  if (data.type & CIMC_ARRAY) {
    int k = 0;
    int n = 0;
    VALUE rbarray = rb_ary_new();

    if (!data.value.array)
      return rb_ary_new();

    n = data.value.array->ft->getSize(data.value.array, &status);
    if (!status.rc) {
      for (k = 0; k < n; ++k) {
        CIMCData element = data.value.array->ft->getElementAt(data.value.array, k, NULL);
        rb_ary_push(rbarray, sfcc_cimcdata_to_value(element));
      }
      return rbarray;
    }
    sfcc_rb_raise_if_error(status, "Can't retrieve array size");
    return Qnil;
  }
  else if (data.type & CIMC_ENC) {    
    switch (data.type) {
    case CIMC_instance:
      return data.value.inst ? Sfcc_wrap_cimc_instance(data.value.inst->ft->clone(data.value.inst, NULL)) : Qnil;
    case CIMC_class:
      return data.value.cls ? Sfcc_wrap_cimc_class(data.value.cls->ft->clone(data.value.cls, NULL)) : Qnil;
    case CIMC_ref:
      return data.value.ref ? Sfcc_wrap_cimc_object_path(data.value.ref->ft->clone(data.value.ref, NULL)) : Qnil;
    case CIMC_args:
    case CIMC_filter:
      return Qnil;
    case CIMC_string:
    case CIMC_numericString:
    case CIMC_booleanString:
    case CIMC_dateTimeString:
    case CIMC_classNameString:
      return data.value.string ? rb_str_new2((char*)data.value.string->hdl) : Qnil;
    case CIMC_dateTime:
      cimstr = data.value.dateTime ? CMGetStringFormat(data.value.dateTime,NULL) : NULL;
      rbval = cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
      if (cimstr) CMRelease(cimstr);
      return rbval;
    }
  }
  else if (data.type & CIMC_SIMPLE) {
    switch (data.type) {
    case CIMC_boolean: return data.value.boolean ? Qtrue : Qfalse;
    case CIMC_char16: return UINT2NUM(data.value.char16);
    }
  }
  else if (data.type & CIMC_INTEGER) {
    switch (data.type) {
    case CIMC_uint8: return UINT2NUM(data.value.uint8);
    case CIMC_sint8: return INT2NUM(data.value.sint8);
    case CIMC_uint16: return UINT2NUM(data.value.uint16);
    case CIMC_sint16: return INT2NUM(data.value.sint16);
    case CIMC_uint32: return UINT2NUM(data.value.uint32);
    case CIMC_sint32: return INT2NUM(data.value.sint32);
    case CIMC_uint64: return UINT2NUM(data.value.uint64);
    case CIMC_sint64: return INT2NUM(data.value.sint64);
    }
  }
  else if (data.type & CIMC_REAL) {
    switch (data.type) {
    case CIMC_real32: return LONG2NUM(data.value.real32);
    case CIMC_real64: return LONG2NUM(data.value.real64);
    }
  }
  rb_raise(rb_eTypeError, "unsupported data data type %d", data.type);
  return Qnil;
}

