
#include <stdarg.h>

#include "sfcc.h"

#include <CimClientLib/cmci.h>
#include <CimClientLib/native.h>
#include <CimClientLib/cmcimacs.h>

#include "cim_string.h"
#include "cim_object_path.h"
#include "cim_enumeration.h"
#include "cim_class.h"
#include "cim_instance.h"
#include "cim_client.h"

VALUE mSfcc;
VALUE mSfccCim;

void Init_sfcc()
{
  /**
   * SBLIM sfcc ruby API
   */
  mSfcc = rb_define_module("Sfcc");
  /**
   * SBLIM sfcc CIMC API
   */
  mSfccCim= rb_define_module_under(mSfcc, "Cim");

  init_cim_string();
  init_cim_object_path();
  init_cim_enumeration();
  init_cim_class();
  init_cim_instance();
  init_cim_client();
}

static VALUE sfcc_status_exception(CMPIStatus status)
{
  switch (status.rc)
  {
  case CMPI_RC_ERR_FAILED:
    return rb_const_get(mSfccCim, rb_intern("ErrorFailed"));
  case CMPI_RC_ERR_ACCESS_DENIED:
    return rb_const_get(mSfccCim, rb_intern("ErrorAcessDenied"));
  case CMPI_RC_ERR_INVALID_NAMESPACE:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidNamespace"));
  case CMPI_RC_ERR_INVALID_PARAMETER:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidParameter"));
  case CMPI_RC_ERR_INVALID_CLASS:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidClass"));
  case CMPI_RC_ERR_NOT_FOUND:
    return rb_const_get(mSfccCim, rb_intern("ErrorNotFound"));
  case CMPI_RC_ERR_NOT_SUPPORTED:
    return rb_const_get(mSfccCim, rb_intern("ErrorNotSupported"));
  case CMPI_RC_ERR_CLASS_HAS_CHILDREN:
    return rb_const_get(mSfccCim, rb_intern("ErrorClassHasChildren"));
  case CMPI_RC_ERR_CLASS_HAS_INSTANCES:
    return rb_const_get(mSfccCim, rb_intern("ErrorClassHasInstances"));
  case CMPI_RC_ERR_INVALID_SUPERCLASS:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidSuperClass"));
  case CMPI_RC_ERR_ALREADY_EXISTS:
    return rb_const_get(mSfccCim, rb_intern("ErrorAlreadyExists"));
  case CMPI_RC_ERR_NO_SUCH_PROPERTY:
    return rb_const_get(mSfccCim, rb_intern("ErrorNoSuchProperty"));
  case CMPI_RC_ERR_TYPE_MISMATCH:
    return rb_const_get(mSfccCim, rb_intern("ErrorTypeMismatch"));
  case CMPI_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED:
    return rb_const_get(mSfccCim, rb_intern("ErrorQueryLanguageNotSupported"));
  case CMPI_RC_ERR_INVALID_QUERY:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidQuery"));
  case CMPI_RC_ERR_METHOD_NOT_AVAILABLE:
    return rb_const_get(mSfccCim, rb_intern("ErrorMethodNotAvailable"));
  case CMPI_RC_ERR_METHOD_NOT_FOUND:
    return rb_const_get(mSfccCim, rb_intern("ErrorMethodNotFound"));
  case CMPI_RC_DO_NOT_UNLOAD:
    return rb_const_get(mSfccCim, rb_intern("ErrorDoNotUnload"));
  case CMPI_RC_NEVER_UNLOAD:
    return rb_const_get(mSfccCim, rb_intern("ErrorNeverUnload"));
  case CMPI_RC_ERROR_SYSTEM:
    return rb_const_get(mSfccCim, rb_intern("ErrorSystem"));
  case CMPI_RC_ERROR:
    return rb_const_get(mSfccCim, rb_intern("ErrorRcError"));
  case CMPI_RC_OK:
  default:
    return Qnil;
  }
}

#define MAX_ERROR_BUFFER 255

void sfcc_rb_raise_if_error(CMPIStatus status, const char *msg, ...)
{
  va_list arg_list;
  char orig_error[MAX_ERROR_BUFFER];
  char error[MAX_ERROR_BUFFER];

  if (!status.rc)
    return;

  va_start(arg_list, msg);
  vsnprintf(orig_error, MAX_ERROR_BUFFER, msg, arg_list);
  va_end(arg_list);

  if (status.msg)
    snprintf(error, MAX_ERROR_BUFFER, "%s : %s", orig_error, status.msg->ft->getCharPtr(status.msg, NULL));
  else
    strcpy(error, orig_error);

  rb_raise(sfcc_status_exception(status), error);
}

const char ** sfcc_value_array_to_string_array(VALUE array)
{
  const char **ret;
  int i = 0;

  if ( !NIL_P(array) && RARRAY_LEN(array) > 0 ) {
    ret = (const char**) malloc(RARRAY_LEN(array)*sizeof(char*));
    for (; i < RARRAY_LEN(array); ++i)
      ret[i] = to_charptr(*(RARRAY_PTR(array) + i));
  }
  else
    ret = NULL;

  return ret;
}

VALUE sfcc_cimdata_to_value(CMPIData data, VALUE client)
{
  CMPIString *cimstr = NULL;
  VALUE rbval;
  CMPIStatus status;

  if (data.type & CMPI_ARRAY) {
    int k = 0;
    int n = 0;
    VALUE rbarray = rb_ary_new();

    if (!data.value.array)
      return rb_ary_new();

    n = data.value.array->ft->getSize(data.value.array, &status);
    if (!status.rc) {
      for (k = 0; k < n; ++k) {
        CMPIData element = data.value.array->ft->getElementAt(data.value.array, k, NULL);
        rb_ary_push(rbarray, sfcc_cimdata_to_value(element, client));
      }
      return rbarray;
    }
    sfcc_rb_raise_if_error(status, "Can't retrieve array size");
    return Qnil;
  }
  else if (data.type & CMPI_ENC) {    
    switch (data.type) {
    case CMPI_instance:
      return data.value.inst ? Sfcc_wrap_cim_instance(data.value.inst->ft->clone(data.value.inst, NULL), client) : Qnil;
    case CMPI_class:
      return data.value.cls ? Sfcc_wrap_cim_class(data.value.cls->ft->clone(data.value.cls, NULL), client) : Qnil;
    case CMPI_ref:
      return data.value.ref ? Sfcc_wrap_cim_object_path(data.value.ref->ft->clone(data.value.ref, NULL), client) : Qnil;
    case CMPI_args:      
      return data.value.args ? sfcc_cimargs_to_hash(data.value.args, client) : Qnil;
    case CMPI_filter:
      return Qnil;
    case CMPI_numericString:
    case CMPI_booleanString:
    case CMPI_dateTimeString:
    case CMPI_classNameString:
      break;
    case CMPI_string:
      return data.value.string ? rb_str_new2((char*)data.value.string->ft->getCharPtr(data.value.string, NULL)) : Qnil;
    case CMPI_charsptr:
      return data.value.chars ? rb_str_new((char*)data.value.dataPtr.ptr, data.value.dataPtr.length) : Qnil;
    case CMPI_dateTime:
      cimstr = data.value.dateTime ? CMGetStringFormat(data.value.dateTime,NULL) : NULL;
      rbval = cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
      if (cimstr) CMRelease(cimstr);
      return rbval;
    }
  }
  else if (data.type & CMPI_SIMPLE) {
    switch (data.type) {
    case CMPI_boolean: return data.value.boolean ? Qtrue : Qfalse;
    case CMPI_char16: return UINT2NUM(data.value.char16);
    }
  }
  else if (data.type & CMPI_INTEGER) {
    switch (data.type) {
    case CMPI_uint8: return UINT2NUM(data.value.uint8);
    case CMPI_sint8: return INT2NUM(data.value.sint8);
    case CMPI_uint16: return UINT2NUM(data.value.uint16);
    case CMPI_sint16: return INT2NUM(data.value.sint16);
    case CMPI_uint32: return UINT2NUM(data.value.uint32);
    case CMPI_sint32: return INT2NUM(data.value.sint32);
    case CMPI_uint64: return UINT2NUM(data.value.uint64);
    case CMPI_sint64: return INT2NUM(data.value.sint64);
    }
  }
  else if (data.type & CMPI_REAL) {
    switch (data.type) {
    case CMPI_real32: return LONG2NUM(data.value.real32);
    case CMPI_real64: return LONG2NUM(data.value.real64);
    }
  }
  else if (data.type & CMPI_null ) {
    return Qnil;
  }
  rb_raise(rb_eTypeError, "unsupported data data type %d", data.type);
  return Qnil;
}

/* callback to add each hash element to a CMPIArgs */
static int hash_to_cimargs_iterator(VALUE key, VALUE value, VALUE extra)
{
  CMPIStatus status;
  CMPIData data;
  CMPIArgs *args = (CMPIArgs *)extra;
  VALUE key_str = rb_funcall(key, rb_intern("to_s"), 0);
  const char *key_cstr = to_charptr(key_str);
  data = sfcc_value_to_cimdata(value);
  status = args->ft->addArg(args, key_cstr, &data.value, data.type);

  if ( !status.rc ) {
    return ST_CONTINUE;
  }

  sfcc_rb_raise_if_error(status, "Can't add argument '%s'", to_charptr(key));
  return ST_STOP;
}

CMPIArgs *sfcc_hash_to_cimargs(VALUE hash)
{
  CMPIArgs *args;
  args = newCMPIArgs(NULL);
  rb_hash_foreach(hash, hash_to_cimargs_iterator, (VALUE)args);
  return args;
}

VALUE sfcc_cimargs_to_hash(CMPIArgs *args, VALUE client)
{
  CMPIArgs *ptr = NULL;
  int i = 0;
  int n = 0;
  VALUE hash;
  CMPIString *argname;
  CMPIData argdata;
  CMPIStatus status;
  char *argname_cstr = NULL;

  //Data_Get_Struct(args, CMPIArgs, ptr);
  ptr = args;
  if (!ptr) {
    rb_raise(rb_eRuntimeError, "Can't retrieve args pointer");
    return Qnil;
  }
  n = ptr->ft->getArgCount(ptr, NULL);
  hash = rb_hash_new();

  for (; i < n; ++i) {
    argname = NULL;
    argdata = ptr->ft->getArgAt(ptr, i, &argname, &status);
    if (!status.rc && argdata.state == CMPI_goodValue ) {
      argname_cstr = argname->ft->getCharPtr(argname, &status);
      if (!argname_cstr) {
        rb_raise(rb_eRuntimeError, "Can't retrieve argument name");
        return Qnil;
      }

      if (!status.rc) {
        rb_hash_aset(hash, rb_funcall(rb_str_new2(argname_cstr), rb_intern("to_sym"), 0), sfcc_cimdata_to_value(argdata, client));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve argument name");
        return Qnil;
      }
    }
    else {
      sfcc_rb_raise_if_error(status, "Can't retrieve argument");
      return Qnil;
    }
  }
  return hash;
}

CMPIData sfcc_value_to_cimdata(VALUE value)
{
  CMPIData data;
  memset(&data, 0, sizeof(CMPIData));
  data.state = CMPI_goodValue;
  data.type = CMPI_null;

  switch (TYPE(value))
  {
  case T_NIL:
    data.type = CMPI_null;
    data.state = CMPI_nullValue;
    break;
  case T_STRING:
    data.type = CMPI_string;
    data.value.string = newCMPIString(to_charptr(value), NULL);
    break;
  case T_TRUE:
    data.type = CMPI_boolean;
    data.value.boolean = 1;
    break;
  case T_FALSE:
    data.type = CMPI_boolean;
    data.value.boolean = 0;
    break;
  case T_FIXNUM:
    data.type = CMPI_sint64;
    data.value.Long = NUM2INT(value);
    break;
/* not yet supported
  case T_BIGNUM:       
    break;
  case T_FLOAT:
    break;
  case T_ARRAY:
    break;
  case T_HASH:
    break;
  case T_SYMBOL:
    */
  case T_DATA:
  default:
    if (CLASS_OF(value) == cSfccCimString) {
      Data_Get_Struct(value, CMPIString, data.value.string);  
      data.type = CMPI_string;
    }
    else {
      data.state = CMPI_badValue;
      data.type = CMPI_null;
      VALUE cname = rb_funcall(rb_funcall(value, rb_intern("class"), 0), rb_intern("to_s"), 0);
      const char *class_name = to_charptr(cname);
      rb_raise(rb_eTypeError, "unsupported data data type: %s", class_name);
      return data;
    }
  }
  return data;
}

/*
 * target_charptr
 * Convert target type to const char *
 */

const char *
to_charptr(VALUE v)
{
  const char *str;
  if (SYMBOL_P(v)) {
    str = rb_id2name(SYM2ID(v));
  }
  else if (TYPE(v) == T_STRING) {
    str = StringValuePtr(v);
  }
  else if (v == Qnil) {
    str = NULL;
  }
  else {
    VALUE v_s = rb_funcall(v, rb_intern("to_s"), 0 );
    str = StringValuePtr(v_s);
  }
  return str;
}
