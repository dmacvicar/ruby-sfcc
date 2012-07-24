#include <stdarg.h>
#include <limits.h>

#include "sfcc.h"

#include "cim_string.h"
#include "cim_data.h"
#include "cim_type.h"
#include "cim_flags.h"
#include "cim_object_path.h"
#include "cim_enumeration.h"
#include "cim_class.h"
#include "cim_instance.h"
#include "cim_client.h"

VALUE mSfcc;
VALUE mSfccCim;
CIMCEnv *cimcEnv;
char *cimcEnvType;

static void
Exit_sfcc(CIMCEnv *env)
{
/*  fprintf(stderr, "Sfcc_dealloc_cimcEnv %p\n", env); */
  if (env) {
    CIMCRelease(env);
    cimcEnv = NULL;
  }
  if (cimcEnvType) {
    free(cimcEnvType);
    cimcEnvType = NULL;
  }
}

void Init_sfcc()
{
  const char *conn;
  VALUE cEnvironment; /* class */
  VALUE value; /* wrapped value */
  int rc;
  char *msg;

#ifdef CIMC_NO_CURL_INIT //defined in cimc/cimc.h since version 2.2.4
  char *rails_env = getenv("RAILS_ENV");
#endif

  /**
   * SBLIM sfcc ruby API
   */
  mSfcc = rb_define_module("Sfcc");
  /**
   * SBLIM sfcc CIMC API
   */
  mSfccCim= rb_define_module_under(mSfcc, "Cim");

  /**
   * alloc CimcEnvironment once, store as const
   */
  cEnvironment = rb_define_class_under(mSfccCim, "CimcEnvironment", rb_cObject);
  conn = getenv("RUBY_SFCC_CONNECTION"); /* "SfcbLocal" or "XML" */
  if (!conn) conn = "XML";
  cimcEnv = NewCIMCEnv(conn,
#ifdef CIMC_NO_CURL_INIT
  /* Don't let sfcc init curl if running in Rails env
   * (http://sourceforge.net/tracker/?func=detail&aid=3435363&group_id=128809&atid=712784) */
        rails_env?CIMC_NO_CURL_INIT:
#endif
        0, &rc, &msg);
  if (!cimcEnv) {
    rb_raise(rb_eLoadError, "Cannot local %s cim client library. %d:%s", conn, rc, msg ? msg : "");
  }
  value = Data_Wrap_Struct(cEnvironment, NULL, Exit_sfcc, cimcEnv);
  rb_define_const(mSfccCim, "CIMC_ENV", value);
  cimcEnvType = strdup(conn);
  rb_define_const(mSfccCim, "CIMC_ENV_TYPE", rb_str_new2(cimcEnvType));

  /**
   * Init other sub-classes
   */
  init_cim_string();
  init_cim_data();
  init_cim_type();
  init_cim_flags();
  init_cim_object_path();
  init_cim_enumeration();
  init_cim_class();
  init_cim_instance();
  init_cim_client();
}

static VALUE sfcc_status_exception(CIMCStatus status)
{
  switch (status.rc)
  {
  case CIMC_RC_ERR_FAILED:
    return rb_const_get(mSfccCim, rb_intern("ErrorFailed"));
  case CIMC_RC_ERR_ACCESS_DENIED:
    return rb_const_get(mSfccCim, rb_intern("ErrorAcessDenied"));
  case CIMC_RC_ERR_INVALID_NAMESPACE:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidNamespace"));
  case CIMC_RC_ERR_INVALID_PARAMETER:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidParameter"));
  case CIMC_RC_ERR_INVALID_CLASS:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidClass"));
  case CIMC_RC_ERR_NOT_FOUND:
    return rb_const_get(mSfccCim, rb_intern("ErrorNotFound"));
  case CIMC_RC_ERR_NOT_SUPPORTED:
    return rb_const_get(mSfccCim, rb_intern("ErrorNotSupported"));
  case CIMC_RC_ERR_CLASS_HAS_CHILDREN:
    return rb_const_get(mSfccCim, rb_intern("ErrorClassHasChildren"));
  case CIMC_RC_ERR_CLASS_HAS_INSTANCES:
    return rb_const_get(mSfccCim, rb_intern("ErrorClassHasInstances"));
  case CIMC_RC_ERR_INVALID_SUPERCLASS:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidSuperClass"));
  case CIMC_RC_ERR_ALREADY_EXISTS:
    return rb_const_get(mSfccCim, rb_intern("ErrorAlreadyExists"));
  case CIMC_RC_ERR_NO_SUCH_PROPERTY:
    return rb_const_get(mSfccCim, rb_intern("ErrorNoSuchProperty"));
  case CIMC_RC_ERR_TYPE_MISMATCH:
    return rb_const_get(mSfccCim, rb_intern("ErrorTypeMismatch"));
  case CIMC_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED:
    return rb_const_get(mSfccCim, rb_intern("ErrorQueryLanguageNotSupported"));
  case CIMC_RC_ERR_INVALID_QUERY:
    return rb_const_get(mSfccCim, rb_intern("ErrorInvalidQuery"));
  case CIMC_RC_ERR_METHOD_NOT_AVAILABLE:
    return rb_const_get(mSfccCim, rb_intern("ErrorMethodNotAvailable"));
  case CIMC_RC_ERR_METHOD_NOT_FOUND:
    return rb_const_get(mSfccCim, rb_intern("ErrorMethodNotFound"));
  case CIMC_RC_DO_NOT_UNLOAD:
    return rb_const_get(mSfccCim, rb_intern("ErrorDoNotUnload"));
  case CIMC_RC_NEVER_UNLOAD:
    return rb_const_get(mSfccCim, rb_intern("ErrorNeverUnload"));
  case CIMC_RC_ERROR_SYSTEM:
    return rb_const_get(mSfccCim, rb_intern("ErrorSystem"));
  case CIMC_RC_ERROR:
    return rb_const_get(mSfccCim, rb_intern("ErrorRcError"));
  case CIMC_RC_OK:
  default:
    return Qnil;
  }
}

#define MAX_ERROR_BUFFER 255

void sfcc_rb_raise_if_error(CIMCStatus status, const char *msg, ...)
{
  va_list arg_list;
  char orig_error[MAX_ERROR_BUFFER];
  char error[MAX_ERROR_BUFFER];

  if (!status.rc)
    return;

  va_start(arg_list, msg);
  vsnprintf(orig_error, MAX_ERROR_BUFFER, msg, arg_list);
  va_end(arg_list);

  if (status.msg) {
    snprintf(error, MAX_ERROR_BUFFER, "%s : %s", orig_error, CIMCGetCharsPtr(status.msg, NULL));
    CIMCRelease(status.msg);
  }
  else
    strcpy(error, orig_error);

  rb_raise(sfcc_status_exception(status), error);
}

char ** sfcc_value_array_to_string_array(VALUE array)
{
  char **ret;
  int i = 0;

  if ( !NIL_P(array) && RARRAY_LEN(array) > 0 ) {
    ret = (char**) malloc(RARRAY_LEN(array)*sizeof(char*));
    for (; i < RARRAY_LEN(array); ++i)
      ret[i] = (char *)to_charptr(*(RARRAY_PTR(array) + i));
  }
  else
    ret = NULL;

  return ret;
}

#define SFCCWRAP(data_type, value_attr, ...) \
  if (data->value.value_attr) { \
    return Sfcc_wrap_cim_ ## data_type( \
          deep_copy ? CIMCClone(data->value.value_attr, NULL) \
                    : data->value.value_attr \
        , ##__VA_ARGS__); \
  }else { \
    return Qnil; \
  }

VALUE sfcc_cimdata_to_value(CIMCData *data, VALUE client, bool deep_copy)
{
  VALUE rbval;
  CIMCStatus status;

  if ((data->state != CIMC_goodValue)
      && (data->state != CIMC_keyValue)) {
    if (data->state & CIMC_nullValue)
      return Qnil;
    if (data->state & CIMC_notFound)
      rb_raise(rb_eRuntimeError, "Value not found");
    if (data->state & CIMC_badValue)
      rb_raise(rb_eArgError, "Bad value");
  }
  if (data->type & CIMC_ARRAY) {
    int k = 0;
    int n = 0;
    VALUE rbarray = rb_ary_new();

    if (!data->value.array)
      return rb_ary_new();

    n = data->value.array->ft->getSize(data->value.array, &status);
    if (!status.rc) {
      for (k = 0; k < n; ++k) {
        CIMCData element = data->value.array->ft->getElementAt(data->value.array, k, NULL);
        rb_ary_push(rbarray, sfcc_cimdata_to_value(&element, client, deep_copy));
      }
      return rbarray;
    }
    sfcc_rb_raise_if_error(status, "Can't retrieve array size");
    return Qnil;
  }
  else if (data->type & CIMC_ENC) {
    switch (data->type) {
    case CIMC_instance:
      SFCCWRAP(instance, inst, client); break;
    case CIMC_ref:
      SFCCWRAP(object_path, ref, client); break;
    case CIMC_class:
      SFCCWRAP(class, cls); break;
    case CIMC_args:
      return data->value.args ?
        sfcc_cimargs_to_hash(data->value.args, client, deep_copy) : Qnil;
    case CIMC_filter:
      return Qnil;
    case CIMC_numericString:
    case CIMC_booleanString:
    case CIMC_dateTimeString:
      break;
    case CIMC_string:
      if (data->value.string) {
        char const *strval = CIMCGetCharsPtr(data->value.string, NULL);
        /* getCharPtr() might return NULL and rb_str_new2 doesn't like that */
        if (strval)
          return rb_str_new2(strval);
      }
      return Qnil;
    case CIMC_chars:
      return data->value.chars ? rb_str_new2(data->value.chars) : Qnil;
    case CIMC_charsptr:
      return data->value.chars ? rb_str_new((char*)data->value.dataPtr.ptr, data->value.dataPtr.length) : Qnil;
    case CIMC_enumeration:
      return data->value.Enum ? Sfcc_wrap_cim_enumeration(data->value.Enum->ft->clone(data->value.Enum, NULL), client) : Qnil;
    case CIMC_dateTime:
      if (data->value.dateTime) {
        CIMCUint64 bintime;
        bintime = data->value.dateTime->ft->getBinaryFormat(data->value.dateTime, NULL);
        rbval = rb_time_new((time_t) (bintime / 1000000L), (time_t) (bintime % 1000000));
      }
      else {
        rbval = Qnil;
      }
      return rbval;
    default:
      rb_raise(rb_eTypeError, "Unhandled type 0x%04x", data->type);
    }
  }
  else if (data->type & CIMC_SIMPLE) {
    switch (data->type) {
    case CIMC_boolean: return data->value.boolean ? Qtrue : Qfalse;
    case CIMC_char16: return UINT2NUM(data->value.char16);
    }
  }
  else if (data->type & CIMC_INTEGER) {
    switch (data->type) {
    case CIMC_uint8: return UINT2NUM(data->value.uint8);
    case CIMC_sint8: return INT2NUM(data->value.sint8);
    case CIMC_uint16: return UINT2NUM(data->value.uint16);
    case CIMC_sint16: return INT2NUM(data->value.sint16);
    case CIMC_uint32: return UINT2NUM(data->value.uint32);
    case CIMC_sint32: return INT2NUM(data->value.sint32);
    case CIMC_uint64: return UINT2NUM(data->value.uint64);
    case CIMC_sint64: return INT2NUM(data->value.sint64);
    }
  }
  else if (data->type & CIMC_REAL) {
    switch (data->type) {
    case CIMC_real32: return rb_float_new(data->value.real32);
    case CIMC_real64: return rb_float_new(data->value.real64);
    }
  }
  else if (data->type & CIMC_null ) {
    return Qnil;
  }
  rb_raise(rb_eTypeError, "unsupported data type %d", data->type);
  return Qnil;
}

/* callback to add each hash element to a CMPIArgs
 * +extra+ is a pointer to allocked CIMCArgs */
static int hash_to_cimargs_iterator(VALUE key, VALUE value, VALUE extra)
{
  CIMCStatus status;
  CIMCData data;
  CIMCArgs *args = (CIMCArgs *)extra;
  const char *key_cstr = to_charptr(key);
  data = sfcc_value_to_cimdata(value);
  if (data.state != CIMC_badValue) {
    status = args->ft->addArg(args, key_cstr, &data.value, data.type);
    /*
    if (TYPE(value) != T_DATA) {
      Sfcc_free_cim_data(&data);
    }
    */
  }

  if ( !status.rc ) {
    return ST_CONTINUE;
  }

  sfcc_rb_raise_if_error(status, "Can't add argument '%s'", to_charptr(key));
  return ST_STOP;
}

CIMCArgs *sfcc_hash_to_cimargs(VALUE hash)
{
  CIMCArgs *args;
  args = cimcEnv->ft->newArgs(cimcEnv, NULL);
  rb_hash_foreach(hash, hash_to_cimargs_iterator, (VALUE)args);
  return args;
}

VALUE sfcc_cimargs_to_hash(CIMCArgs *args, VALUE client, bool deep_copy)
{
  int i = 0;
  int n = 0;
  VALUE hash;
  CIMCString *argname;
  CIMCData argdata;
  CIMCStatus status;
  char *argname_cstr = NULL;

  /* Data_Get_Struct(value, CIMCArgs, args); */
  if (!args) {
    rb_raise(rb_eRuntimeError, "Can't retrieve args pointer");
    return Qnil;
  }
  n = args->ft->getArgCount(args, NULL);
  hash = rb_hash_new();

  for (; i < n; ++i) {
    argname = NULL;
    argdata = args->ft->getArgAt(args, i, &argname, &status);
    if (!status.rc && argdata.state == CIMC_goodValue ) {
      argname_cstr = CIMCGetCharsPtr(argname, &status);
      if (!argname_cstr) {
        rb_raise(rb_eRuntimeError, "Can't retrieve argument name");
        return Qnil;
      }

      if (!status.rc) {
        rb_hash_aset(hash, rb_funcall(rb_str_new2(argname_cstr), rb_intern("to_sym"), 0), sfcc_cimdata_to_value(&argdata, client, deep_copy));
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

#define STOREDATAVAL(data_type_suf, struct_suf, struct_attr, value_attr, cimc_suf) \
  if (CLASS_OF(value) == cSfccCim ## data_type_suf) { \
    rb_sfcc_ ## struct_suf  *obj; \
    Data_Get_Struct(value, rb_sfcc_ ## struct_suf, obj); \
    data.value.value_attr = obj->struct_attr; \
    if (data.value.value_attr == NULL) { \
      data.type = CIMC_null; \
      data.state = CIMC_nullValue; \
    }else { \
      data.type = CIMC_ ## cimc_suf; \
    } \
  }

CIMCData sfcc_value_to_cimdata(VALUE value)
{
  CIMCData data;
  memset(&data, 0, sizeof(CIMCData));
  data.state = CIMC_goodValue;
  data.type = CIMC_null;

  switch (TYPE(value))
  {
  case T_NIL:
    data.type = CIMC_null;
    data.state = CIMC_nullValue;
    break;
  case T_STRING:
    data.type = CIMC_string;
    data.value.string = cimcEnv->ft->newString(cimcEnv, to_charptr(value), NULL);
    break;
  case T_TRUE:
    data.type = CIMC_boolean;
    data.value.boolean = 1;
    break;
  case T_FALSE:
    data.type = CIMC_boolean;
    data.value.boolean = 0;
    break;
  case T_FIXNUM:
    data.type = CIMC_sint64;
    data.value.Long = NUM2INT(value);
    break;
  case T_FLOAT:
    data.type = CIMC_real64;
    data.value.real64 = NUM2DBL(value);
    break;
  case T_BIGNUM:
    data.type = CIMC_sint64;
    data.value.Long = NUM2LL(value);
    break;
/* not yet supported
  case T_HASH:
    break;
  case T_SYMBOL:
    */
  case T_ARRAY: {
    CIMCCount i = 0;
    int len = RARRAY_LEN(value);
    CIMCType type = CIMC_string; /* sfcc can't handle CIMC_null */
    VALUE array_value;
    CIMCData array_data;    
    if (len > 0) {
      /* try to deduce type from first array element */
      array_value = rb_ary_entry(value, 0);
      array_data = sfcc_value_to_cimdata(array_value);
      type = array_data.type;
      if (TYPE(array_value) != T_DATA) {
        Sfcc_free_cim_data(&array_data);
      }
    }
    data.type = type | CIMC_ARRAY;
    data.state = CIMC_goodValue;
    data.value.array = cimcEnv->ft->newArray(cimcEnv, len, type, NULL);
    if (len > 0) {
      data.value.array->ft->setElementAt(data.value.array, i++,
          &(array_data.value), array_data.type);
    }
    for (; i < (typeof(i)) len; ++i) {
      array_value = rb_ary_entry(value, 0);
      array_data = sfcc_value_to_cimdata(array_value);
      data.value.array->ft->setElementAt(data.value.array, i,
          &(array_data.value), array_data.type);
      if (TYPE(array_value) != T_DATA) {
        Sfcc_free_cim_data(&array_data);
      }
    }
    break;
  }
  case T_DATA:
  default:
    if (CLASS_OF(value) == cSfccCimString) {
      Data_Get_Struct(value, CIMCString, data.value.string);
      if (data.value.string == NULL) { /* packed a NULL pointer ? */
        data.type = CIMC_null;
        data.state = CIMC_nullValue;
      }else {
        data.type = CIMC_string;
      }
    }else if (CLASS_OF(value) == cSfccCimData) {
      CIMCData *tmp;
      Data_Get_Struct(value, CIMCData, tmp);
      data = *tmp;
    }else STOREDATAVAL(Instance, instance, inst, inst, instance)
    else  STOREDATAVAL(ObjectPath, object_path, op, ref, ref)
    else  STOREDATAVAL(Enumeration, enumeration, enm, Enum, enumeration)
    else if (CLASS_OF(value) == cSfccCimClass) {
      Data_Get_Struct(value, CIMCClass, data.value.cls);
      if (data.value.cls == NULL) {
        data.type = CIMC_null;
        data.state = CIMC_nullValue;
      }else {
        data.type = CIMC_class;
      }
    }else {
      VALUE cname;
      const char *class_name;
      data.state = CIMC_badValue;
      data.type = CIMC_null;
      cname = rb_funcall(rb_funcall(value, rb_intern("class"), 0), rb_intern("to_s"), 0);
      class_name = to_charptr(cname);
      rb_raise(rb_eTypeError, "unsupported data type: %s", class_name);
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

/**
 * converts a CIMCArray to rbArray
 */

VALUE
sfcc_cimcarray_to_rubyarray(CIMCArray *array, VALUE client, bool deep_copy)
{
  CIMCCount size;
  CIMCCount i;
  CIMCStatus st = { 0, NULL };
  VALUE ary;
  if (!array)
    rb_raise(rb_eArgError, "Cannot convert NULL array");
  size = array->ft->getSize(array, &st);
  sfcc_rb_raise_if_error(st, "Can't get array size");
  ary = rb_ary_new2(size);
  for (i = 0; i < size; ++i) {
    CIMCData data;
    VALUE value;
    array->ft->getElementAt(array,i,&st);
    sfcc_rb_raise_if_error(st, "Can't get array element %d of %d", i, size);
    value = sfcc_cimdata_to_value(&data, client, deep_copy);
    rb_ary_store(ary,i,value);
  }
  return ary;
}

