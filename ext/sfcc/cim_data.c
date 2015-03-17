#include <stdint.h>
#include <stdbool.h>
#include "cim_class.h"
#include "cim_instance.h"
#include "cim_enumeration.h"
#include "cim_object_path.h"
#include "cim_string.h"
#include "cim_type.h"
#include "sfcc.h"
#include "cim_data.h"

VALUE cSfccCimData;

/**
 * CIMCData needs to be wrapped in this struct together with information
 * whether clear_cimdata should be called upon it on destruction
 */
typedef struct {
  CIMCData data;
  /* Says, whether data contains value, that needs to be freed
   * upon Cim::Data destruction or not.
   * When true, the value will not be freed.
   */
  bool reference;
}rb_sfcc_data;

/**
 * just releases kept value, unsets the value and sets
 * the state to CIMC_nullValue
 */
static void clear_cimdata(CIMCData *d)
{
  if (  ((d)->type < CIMC_instance  || (d)->type > CIMC_dateTime)
     && ((d)->type < CIMC_instanceA || (d)->type > CIMC_dateTimeA)
     && d->type != CIMC_chars && d->type != CIMC_charsptr
     && !(d->type & CIMC_ARRAY)) return;

  if (d->state != CIMC_goodValue && d->state != CIMC_keyValue) return;

  if (d->type & CIMC_ARRAY) {
    d->value.array->ft->release(d->value.array);
  }else {
    switch (d->type) {
      case CIMC_instance: d->value.inst->ft->release(d->value.inst); break;
      case CIMC_ref: d->value.ref->ft->release(d->value.ref); break;
      //case CIMC_args: not supported
      case CIMC_class: d->value.cls->ft->release(d->value.cls); break;
      //case CIMC_filter: d->value.filter->ft->release(d->value.filter); break;
      case CIMC_enumeration: d->value.Enum->ft->release(d->value.Enum); break;
      case CIMC_string: d->value.string->ft->release(d->value.string); break;
      case CIMC_dateTime: d->value.dateTime->ft->release(d->value.dateTime); break;
      case CIMC_chars: free(d->value.chars); break;
      case CIMC_charsptr:
         free(d->value.dataPtr.ptr);
         d->value.dataPtr.length = 0;
         break;
      default: break;
    }
  }
  d->state = CIMC_nullValue;
  memset(&d->value, 0, sizeof(CIMCValue));
}

/**
 * frees the wrapping structure and possibly referenced value
 */
static void
dealloc(rb_sfcc_data *rd)
{
  if (!rd->reference) {
    clear_cimdata(&rd->data);
  }
  free(rd);
}

/**
 * sets the Cim::Data's type
 *
 * it deallocates CIMCData's value in case the type is different from current
 * one
 */
static void do_set_type(rb_sfcc_data *data, VALUE type)
{
  char buf[100];
  VALUE type_except;
  snprintf(buf, 100, "expected Type instance, Symbol,"
          " String or Fixnum, not: %s",
          to_charptr(rb_obj_class(type)));
  type_except = rb_exc_new2(rb_eTypeError, buf);

  switch (TYPE(type)) {
    case T_STRING:
      type = rb_intern(StringValuePtr(type));
    case T_SYMBOL:
      type = rb_const_get(cSfccCimType, type); break;
    case T_DATA:
      if (CLASS_OF(type) != cSfccCimType) {
        rb_exc_raise(type_except);
        return;
      }
      type = INT2FIX(Sfcc_rb_type_to_cimtype(type));
    case T_FIXNUM:
      if (!Sfcc_cim_type_to_cstr(FIX2UINT(type))) return;
      break;
    default:
      data->data.state = CIMC_badValue;
      rb_exc_raise(type_except);
      return;
  }
  if (data->data.type != (CIMCType) FIX2UINT(type)) {
    if (!data->reference) clear_cimdata(&data->data);
    data->reference = false;
    data->data.type = (CIMCType) FIX2UINT(type);
    data->data.state = CIMC_nullValue;
  }
}

/**
 * helper macro used to set value of CIMCData in case of wrapped object
 * in structure rb_sfcc_...
 *
 * @param data_type_suf is a suffix of ruby klass beginning with cSfccCim
 * @param struct_suf is a suffix for rb_sfcc_ structure for corresponding type
 * @param struct_attr is attribute name used to access value in this structure
 * @param value_attr is attribute name used to access value of corresponding type
 *                   in CIMCValue
 * @param cimc_suf is a suffix of cimc type beggining with CIMC_
 */
#define STORE_DATA_VAL(data_type_suf, struct_suf, struct_attr, value_attr, cimc_suf) \
  if (CLASS_OF(value) == cSfccCim ## data_type_suf) { \
    if (d->type == CIMC_ ## cimc_suf) { \
      rb_sfcc_ ## struct_suf * tmp; \
      Data_Get_Struct(value, rb_sfcc_ ## struct_suf, tmp); \
      d->value.value_attr = tmp->struct_attr; \
      data->reference = true; \
    }else { \
      d->state = CIMC_badValue; \
      rb_raise(rb_eTypeError, #data_type_suf \
            " object can only be set for " #cimc_suf " type"); \
    } \
  } \

/**
 * sets the value of Cim::Data
 *
 * @param value will be transformed to cimc data and then stored
 */
static void do_set_value(rb_sfcc_data *data, VALUE value)
{
  // dealloc previously kept value
  CIMCData *d;
  if (!data->reference) clear_cimdata(&data->data);
  d = &data->data;
  data->reference = false;
  d->state = CIMC_goodValue;

  switch (TYPE(value)) {
    case T_NIL:
      d->state = CIMC_nullValue;
      break;

    case T_SYMBOL:  // first make a string out of symbol
      value = rb_any_to_s(value);
    case T_STRING:
      if ((d->type & (CIMC_UINT | CIMC_char16))) {
        // handle unsigned numbers passed as string
        uint64_t tmp = NUM2ULL(rb_str2inum(value, 10));
        switch (d->type) {
          case CIMC_uint32: d->value.uint32 = tmp; break;
          case CIMC_uint16: d->value.uint16 = tmp; break;
          case CIMC_uint8:  d->value.uint8 = tmp; break;
          default: d->value.uint64 = tmp; break;
        }
      }else if (d->type & CIMC_SINT) {
        // handle signed numbers passed as string
        int64_t tmp = NUM2LL(rb_str2inum(value, 10));
        switch (d->type) {
          case CIMC_sint32: d->value.sint32 = tmp; break;
          case CIMC_sint16: d->value.sint16 = tmp; break;
          case CIMC_sint8: d->value.sint8 = tmp; break;
          default: d->value.sint64 = tmp; break;
        }
      }else if (d->type & CIMC_REAL) {
        // handle reals passed as string
        double tmp = NUM2LL(rb_Float(value));
        switch (d->type) {
          case CIMC_real32: d->value.real32 = tmp; break;
          default: d->value.real64 = tmp; break;
        }
      }else if (d->type == CIMC_string) {
        d->value.string = cimcEnv->ft->newString(
                cimcEnv, StringValuePtr(value), NULL);
      }else if (d->type == CIMC_chars) {
        d->value.chars = strdup(StringValuePtr(value));
      }else if (d->type == CIMC_charsptr) {
        d->value.dataPtr.ptr = strdup(StringValuePtr(value));
        d->value.dataPtr.length = rb_funcall(
                value, rb_intern("length"), 0);
      }else {
        d->state = CIMC_badValue;
        rb_raise(rb_eTypeError, "unsupported data type(%s) for value"
                ", when data.type set to \"%s\"",
                to_charptr(rb_obj_class(value)),
                Sfcc_cim_type_to_cstr(d->type));
      }
      break;

    case T_TRUE:
    case T_FALSE:
      if (d->type != CIMC_boolean) {
        d->state = CIMC_badValue;
        rb_raise(rb_eTypeError, "boolean values are supported only"
               " for CIMC_boolean type");
        d->value.boolean = TYPE(value) == T_TRUE;
      }
      break;

    case T_FIXNUM:
    case T_BIGNUM:
    case T_FLOAT:
      if ((d->type & (CIMC_UINT | CIMC_char16))) {
        // handle unsigned numbers
        uint64_t tmp = NUM2ULL(value);
        switch (d->type) {
          case CIMC_uint32: d->value.uint32 = tmp; break;
          case CIMC_uint16: d->value.uint16 = tmp; break;
          case CIMC_uint8:  d->value.uint8 = tmp; break;
          default: d->value.uint64 = tmp; break;
        }
      }else if (d->type & CIMC_SINT) {
        // handle signed numbers
        int64_t tmp = NUM2LL(value);
        switch (d->type) {
          case CIMC_sint32: d->value.sint32 = tmp; break;
          case CIMC_sint16: d->value.sint16 = tmp; break;
          case CIMC_sint8: d->value.sint8 = tmp; break;
          default: d->value.sint64 = tmp; break;
        }
      }else if (d->type & CIMC_REAL) {
        // handle real numbers
        double tmp = NUM2DBL(value);
        switch (d->type) {
          case CIMC_real32: d->value.real32 = tmp; break;
          default: d->value.real64 = tmp; break;
        }

      // try to convert numbers to string
      }else if (d->type == CIMC_string) {
        VALUE num_s = sfcc_numeric_to_str(value);
        char *str = StringValuePtr(num_s);
        d->value.string = cimcEnv->ft->newString(
                                                 cimcEnv, str, NULL);
      }else if (d->type == CIMC_chars) {
        VALUE num_s = sfcc_numeric_to_str(value);
        char *str = StringValuePtr(num_s);
        d->value.chars = strdup(str);
      }else if (d->type == CIMC_charsptr) {
        VALUE num_s = sfcc_numeric_to_str(value);
        VALUE str = StringValue(num_s);
        d->value.dataPtr.ptr = strdup(RSTRING_PTR(str));
        d->value.dataPtr.length = RSTRING_LEN(str);
      }else {
        d->state = CIMC_badValue;
        rb_raise(rb_eTypeError, "unsupported data type(%s) for value"
                ", when data.type set to \"%s\"",
                to_charptr(rb_obj_class(value)),
                Sfcc_cim_type_to_cstr(d->type));
      }
      break;

    case T_ARRAY:
      if (d->type & CIMC_ARRAY) {
        if (!(d->value.array = sfcc_rubyarray_to_cimcarray(
                        value, &d->type)))
            d->state = CIMC_badValue;
      }else {
        rb_raise(rb_eTypeError, "unsupported data type(%s) for value"
                ", when data.type set to \"%s\"",
                to_charptr(rb_obj_class(value)),
                Sfcc_cim_type_to_cstr(d->type));
      }
      break;

    /* not yet supported
    case T_HASH:
      // this would result in CIMCArgs ... is this really needed?
    break;
    */

    case T_DATA:
      /* handle wrapped objects
       * these will be destructed upon garbage collection
       * by corresponding destructors of their ruby objects
       *
       * so data->reference must by set to true
       */
    default:
      if (CLASS_OF(value) == cSfccCimString) {
        if (d->type == CIMC_string) {
          Data_Get_Struct(value, CIMCString, d->value.string);
          data->reference = true;
        }else {
          d->state = CIMC_badValue;
          rb_raise(rb_eTypeError, "string value can be set only for"
                 " String type");
        }
      }else STORE_DATA_VAL(Instance, instance, inst, inst, instance)
      else  STORE_DATA_VAL(ObjectPath, object_path, op, ref, ref)
      else  STORE_DATA_VAL(Enumeration, enumeration, enm, Enum, enumeration)
      else if (CLASS_OF(value) == cSfccCimClass) {
        if (d->type == CIMC_class) {
          Data_Get_Struct(value, CIMCClass, d->value.cls);
          data->reference = true;
        }else {
          d->state = CIMC_badValue;
          rb_raise(rb_eTypeError,
                  "class can only be set for Class type");
        }
      }else {
        VALUE cname;
        const char *class_name;
        d->state = CIMC_badValue;
        cname = rb_funcall(rb_funcall(value, rb_intern("class"), 0),
                rb_intern("to_s"), 0);
        class_name = to_charptr(cname);
        rb_raise(rb_eTypeError, "unsupported data type: %s",
                class_name);
      }
  }
}

/**
 * call-seq:
 *  new(type, value)                   -> Cim::Data
 *  new("Reference", op)               -> Cim::Data
 *  new("Cim::Type::String", "string") -> Cim::Data
 *
 * Creates a new data object from +type+ and +value+.
 * +value+'s type must correspond to +type+, otherwise a +TypeError+ is raised.
 * Also if +value+ is an +Array+, then all of its elements must be of the
 * same type.
 *
 * +type+ can be given as +String+ or as +Symbol+ from +Cim::Type+.
 */
static VALUE new(VALUE klass, VALUE type, VALUE value)
{
  VALUE res = Qnil;
  CIMCData tmp;
  tmp.state = CIMC_nullValue;
  tmp.type = CIMC_null;
  if ((res = Sfcc_wrap_cim_data(&tmp)) != Qnil) {
    rb_sfcc_data *data;
    Data_Get_Struct(res, rb_sfcc_data, data);
    do_set_type(data, type);
    do_set_value(data, value); //make a deep copy of value
    rb_obj_call_init(res, 0, NULL);
  }
  return res;
}

/**
 * call-seq:
 *  from_value(123)       -> Cim::Data
 *  from_value("string")  -> Cim::Data
 *  from_value([1, 2, 3]) -> Cim::Data
 *
 * Created a new data object from +value+ only. The +Cim::Type+ is guessed
 * from type of +value+:
 *  * +String+             -> +Cim::Type::String+
 *  * +Fixnum+ or +Bignum+ -> +Cim::Type::SInt64+
 *  * +Float+              -> +Cim::Type::Real64+
 *  * +Array+ containing values of type xy -> Cim::Type::xyA
 * any wrapped cimc objects in Cim namespace are also supported
 */
static VALUE from_value(VALUE klass, VALUE value)
{
  VALUE res = Qnil;
  CIMCData tmp;
  tmp.state = CIMC_nullValue;
  tmp.type = CIMC_null;
  if ((res = Sfcc_wrap_cim_data(&tmp)) != Qnil) {
    rb_sfcc_data *data;
    Data_Get_Struct(res, rb_sfcc_data, data);
    data->data = sfcc_value_to_cimdata(value);
    /* if value is a cimc wrapped object, then set the reference */
    data->reference = TYPE(value) == T_DATA ? true:false;
  }
  return res;
}

/**
 * call-seq:
 *   state()
 *
 * Get the state of the data
 */
static VALUE state(VALUE self)
{
  rb_sfcc_data *data;
  Data_Get_Struct(self, rb_sfcc_data, data);
  return UINT2NUM(data->data.state);
}

/**
 * call-seq:
 *   type() -> Cim::Type
 *
 * Get the type of the data
 */
static VALUE type(VALUE self)
{
  rb_sfcc_data *data;
  Data_Get_Struct(self, rb_sfcc_data, data);
  return Sfcc_wrap_cim_type(data->data.type);
}

/**
 * call-seq:
 *  type = "Reference"
 *  type = Cim::Type::Reference
 *
 * Sets the type of +Cim::Data+.
 *
 * +type+ can be either +String+ or a +Symbol+ from +Cim::Type+ namespace.
 */
static VALUE set_type(VALUE self, VALUE type)
{
  rb_sfcc_data *data;
  Data_Get_Struct(self, rb_sfcc_data, data);
  do_set_type(data, type);
  return type;
}

/**
 * call-seq:
 *   value()
 *
 * Get the value of the data
 */
static VALUE value(VALUE self)
{
  rb_sfcc_data *data;
  Data_Get_Struct(self, rb_sfcc_data, data);
  return sfcc_cimdata_to_value(&data->data, Qnil);
}

/**
 * call-seq:
 *  value=(10)
 *  value=(["abc", "def"])
 *
 * Set value of data.
 *
 * +value+ must be convertible to data's type, otherwise a +TypeError+ will
 * be raised.
 */
static VALUE set_value(VALUE self, VALUE value)
{
  rb_sfcc_data *data;
  Data_Get_Struct(self, rb_sfcc_data, data);
  do_set_value(data, value);
  return value;
}

/**
 * call-seq:
 *   state_is(state) -> Boolean
 *
 * Check the value state
 */
static VALUE state_is(VALUE self, VALUE state)
{
  rb_sfcc_data *data;
  Check_Type(state, T_FIXNUM);
  Data_Get_Struct(self, rb_sfcc_data, data);
  if (data->data.state == FIX2INT(state))
    return Qtrue;
  return Qfalse;
}


VALUE
Sfcc_wrap_cim_data(CIMCData *data)
{
  rb_sfcc_data *d = malloc(sizeof(rb_sfcc_data));
  if (!d) {
    rb_raise(rb_eNoMemError, "failed to allocate data");
    return Qnil;
  }
  d->data = *data;
  d->reference = true;
  return Data_Wrap_Struct(cSfccCimData, NULL, dealloc, d);
}

void Sfcc_clear_cim_data(CIMCData *data)
{
  clear_cimdata(data);
}

void init_cim_data()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * data on the CIM namespace
   */
  VALUE klass = rb_define_class_under(cimc, "Data", rb_cObject);
  cSfccCimData = klass;

  rb_define_singleton_method(klass, "new", new, 2);
  rb_define_singleton_method(klass, "from_value", from_value, 1);
  rb_define_method(klass, "state", state, 0);
  rb_define_method(klass, "type", type, 0);
  rb_define_method(klass, "type=", set_type, 1);
  rb_define_method(klass, "value", value, 0);
  rb_define_method(klass, "value=", set_value, 1);

  /* Value state */
  rb_define_method(klass, "state_is", state_is, 1);

  rb_define_const(klass, "Good", INT2FIX(CIMC_goodValue)); /* (0) */
  rb_define_const(klass, "Null", INT2FIX(CIMC_nullValue)); /* (1<<8) */
  rb_define_const(klass, "Key", INT2FIX(CIMC_keyValue)); /*  (2<<8) */
  rb_define_const(klass, "NotFound", INT2FIX(CIMC_notFound)); /*  (4<<8) */
  rb_define_const(klass, "Bad", INT2FIX(CIMC_badValue)); /*  (0x80<<8) */
}
