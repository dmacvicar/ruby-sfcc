/*
 * Cim::Type
 * 
 */

#include "cim_type.h"

typedef struct {
  CIMCType type;
} rb_sfcc_type;

static void
dealloc(rb_sfcc_type *type)
{
  free(type);
}

CIMCType Sfcc_rb_type_to_cimtype(VALUE self)
{
  rb_sfcc_type *type;
  Data_Get_Struct(self, rb_sfcc_type, type);
  return type->type;
}

/**
 * call-seq:
 *   to_i() -> Integer
 *
 * Integer representation of type
 */
static VALUE to_i(VALUE self)
{
  return INT2FIX(Sfcc_rb_type_to_cimtype(self));
}

char const * Sfcc_cim_type_to_cstr(CIMCType type) {
  const char *s = NULL;
  switch (type) {
    case CIMC_null: s = "null"; break;
    
    case CIMC_boolean: s = "boolean"; break;
    case CIMC_char16: s = "char16"; break;

    case CIMC_real32: s = "real32"; break;
    case CIMC_real64: s = "real64"; break;

    case CIMC_uint8: s = "uint8"; break;
    case CIMC_uint16: s = "uint16"; break;
    case CIMC_uint32: s = "uint32"; break;
    case CIMC_uint64: s = "uint64"; break;
    case CIMC_sint8: s = "sint8"; break;
    case CIMC_sint16: s = "sint16"; break;
    case CIMC_sint32: s = "sint32"; break;
    case CIMC_sint64: s = "sint64"; break;

    case CIMC_instance: s = "instance"; break;
    case CIMC_ref: s = "ref"; break;
    case CIMC_args: s = "args"; break;
    case CIMC_class: s = "class"; break;
    case CIMC_filter: s = "filter"; break;
    case CIMC_enumeration: s = "enumeration"; break;
    case CIMC_string: s = "string"; break;
    case CIMC_chars: s = "chars"; break;
    case CIMC_dateTime: s = "datetime"; break;
    case CIMC_ptr: s = "ptr"; break;
    case CIMC_charsptr: s = "charsptr"; break;

    /* Array */
    case CIMC_booleanA: s = "boolean[]"; break;
    case CIMC_char16A: s = "char16[]"; break;

    case CIMC_real32A: s = "real32[]"; break;
    case CIMC_real64A: s = "real64[]"; break;

    case CIMC_uint8A: s = "uint8[]"; break;
    case CIMC_uint16A: s = "uint16[]"; break;
    case CIMC_uint32A: s = "uint32[]"; break;
    case CIMC_uint64A: s = "uint64[]"; break;
    case CIMC_sint8A: s = "sint8[]"; break;
    case CIMC_sint16A: s = "sint16[]"; break;
    case CIMC_sint32A: s = "sint32[]"; break;
    case CIMC_sint64A: s = "sint64[]"; break;

    case CIMC_instanceA: s = "instance[]"; break;
    case CIMC_refA: s = "ref[]"; break;
    case CIMC_stringA: s = "string[]"; break;
    case CIMC_charsA: s = "chars[]"; break;
    case CIMC_dateTimeA: s = "datetime[]"; break;
    case CIMC_ptrA: s = "ptr[]"; break;
    case CIMC_charsptrA: s = "charsptr[]"; break;
  default:
    rb_raise(rb_eTypeError, "Unknown Cim::Type");
    break;
  }
  return s;
}

/**
 * call-seq:
 *   to_s() -> String
 *
 * String representation of type
 */
static VALUE to_s(VALUE self)
{
  char const * s = Sfcc_cim_type_to_cstr(Sfcc_rb_type_to_cimtype(self));
  if (s) {
    return rb_str_new2(s);
  }
  return Qnil;
}


VALUE
Sfcc_wrap_cim_type(CIMCType type)
{
  rb_sfcc_type *rst = (rb_sfcc_type *)malloc(sizeof(rb_sfcc_type));
  if (!rst)
    rb_raise(rb_eNoMemError, "Cannot alloc rb_sfcc_type");
  rst->type = type;
  return Data_Wrap_Struct(cSfccCimType, NULL, dealloc, rst);
}


VALUE cSfccCimType;
void init_cim_type()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * data on the CIM namespace
   */
  VALUE klass = rb_define_class_under(cimc, "Type", rb_cObject);
  cSfccCimType = klass;

  rb_define_method(klass, "to_s", to_s, 0);
  rb_define_method(klass, "to_i", to_i, 0);

  /**
   * data on the CIM namespace
   */

  rb_define_const(klass, "Null",        INT2FIX(CIMC_null));

  rb_define_const(klass, "SIMPLE",      INT2FIX(CIMC_SIMPLE));
  rb_define_const(klass, "Boolean",     INT2FIX(CIMC_boolean));
  rb_define_const(klass, "Char16",      INT2FIX(CIMC_char16));

  rb_define_const(klass, "REAL",        INT2FIX(CIMC_REAL));
  rb_define_const(klass, "Real32",      INT2FIX(CIMC_real32));
  rb_define_const(klass, "Real64",      INT2FIX(CIMC_real64));

  rb_define_const(klass, "UINT",        INT2FIX(CIMC_UINT));
  rb_define_const(klass, "UInt8",       INT2FIX(CIMC_uint8));
  rb_define_const(klass, "UInt16",      INT2FIX(CIMC_uint16));
  rb_define_const(klass, "UInt32",      INT2FIX(CIMC_uint32));
  rb_define_const(klass, "UInt64",      INT2FIX(CIMC_uint64));
  rb_define_const(klass, "SINT",        INT2FIX(CIMC_SINT));
  rb_define_const(klass, "SInt8",       INT2FIX(CIMC_sint8));
  rb_define_const(klass, "SInt16",      INT2FIX(CIMC_sint16));
  rb_define_const(klass, "SInt32",      INT2FIX(CIMC_sint32));
  rb_define_const(klass, "SInt64",      INT2FIX(CIMC_sint64));
  rb_define_const(klass, "INTEGER",     INT2FIX(CIMC_INTEGER));

  rb_define_const(klass, "ENC",         INT2FIX(CIMC_ENC));
  rb_define_const(klass, "Instance",    INT2FIX(CIMC_instance));
  rb_define_const(klass, "Reference",   INT2FIX(CIMC_ref));
  rb_define_const(klass, "Args",        INT2FIX(CIMC_args));
  rb_define_const(klass, "Class",       INT2FIX(CIMC_class));
  rb_define_const(klass, "Filter",      INT2FIX(CIMC_filter));
  rb_define_const(klass, "Enumeration", INT2FIX(CIMC_enumeration));
  rb_define_const(klass, "String",      INT2FIX(CIMC_string));
  rb_define_const(klass, "Chars",       INT2FIX(CIMC_chars));
  rb_define_const(klass, "DateTime",    INT2FIX(CIMC_dateTime));
  rb_define_const(klass, "Ptr",         INT2FIX(CIMC_ptr));
  rb_define_const(klass, "CharsPtr",    INT2FIX(CIMC_charsptr));

  rb_define_const(klass, "ARRAY",       INT2FIX(CIMC_ARRAY)); /*        ((1)<<13) */
  rb_define_const(klass, "SIMPLEA",     INT2FIX(CIMC_SIMPLEA)); /*      (CIMC_ARRAY | CIMC_SIMPLE) */
  rb_define_const(klass, "BooleanA",    INT2FIX(CIMC_booleanA)); /*     (CIMC_ARRAY | CIMC_boolean) */
  rb_define_const(klass, "Char16A",     INT2FIX(CIMC_char16A)); /*      (CIMC_ARRAY | CIMC_char16) */

  rb_define_const(klass, "REALA",       INT2FIX(CIMC_REALA)); /*        (CIMC_ARRAY | CIMC_REAL) */
  rb_define_const(klass, "Real32A",     INT2FIX(CIMC_real32A)); /*      (CIMC_ARRAY | CIMC_real32) */
  rb_define_const(klass, "Real64A",     INT2FIX(CIMC_real64A)); /*      (CIMC_ARRAY | CIMC_real64) */

  rb_define_const(klass, "UNITA",       INT2FIX(CIMC_UINTA)); /*        (CIMC_ARRAY | CIMC_UINT) */
  rb_define_const(klass, "UInt8A",      INT2FIX(CIMC_uint8A)); /*       (CIMC_ARRAY | CIMC_uint8) */
  rb_define_const(klass, "UInt16A",     INT2FIX(CIMC_uint16A)); /*      (CIMC_ARRAY | CIMC_uint16) */
  rb_define_const(klass, "UInt32A",     INT2FIX(CIMC_uint32A)); /*      (CIMC_ARRAY | CIMC_uint32) */
  rb_define_const(klass, "UInt64A",     INT2FIX(CIMC_uint64A)); /*      (CIMC_ARRAY | CIMC_uint64) */
  rb_define_const(klass, "SINTA",       INT2FIX(CIMC_SINTA)); /*        (CIMC_ARRAY | CIMC_SINT) */
  rb_define_const(klass, "SInt8A",      INT2FIX(CIMC_sint8A)); /*       (CIMC_ARRAY | CIMC_sint8) */
  rb_define_const(klass, "SInt16A",     INT2FIX(CIMC_sint16A)); /*      (CIMC_ARRAY | CIMC_sint16) */
  rb_define_const(klass, "SInt32A",     INT2FIX(CIMC_sint32A)); /*      (CIMC_ARRAY | CIMC_sint32) */
  rb_define_const(klass, "SInt64A",     INT2FIX(CIMC_sint64A)); /*      (CIMC_ARRAY | CIMC_sint64) */
  rb_define_const(klass, "INTEGERA",    INT2FIX(CIMC_INTEGERA)); /*     (CIMC_ARRAY | CIMC_INTEGER) */

  rb_define_const(klass, "ENCA",        INT2FIX(CIMC_ENCA)); /*         (CIMC_ARRAY | CIMC_ENC) */
  rb_define_const(klass, "StringA",     INT2FIX(CIMC_stringA)); /*      (CIMC_ARRAY | CIMC_string) */
  rb_define_const(klass, "CharsA",      INT2FIX(CIMC_charsA)); /*       (CIMC_ARRAY | CIMC_chars) */
  rb_define_const(klass, "DataTimeA",   INT2FIX(CIMC_dateTimeA)); /*    (CIMC_ARRAY | CIMC_dateTime) */
  rb_define_const(klass, "InstanceA",   INT2FIX(CIMC_instanceA)); /*    (CIMC_ARRAY | CIMC_instance) */
  rb_define_const(klass, "ReferenceA",  INT2FIX(CIMC_refA)); /*         (CIMC_ARRAY | CIMC_ref) */
  rb_define_const(klass, "PtrA",        INT2FIX(CIMC_ptrA)); /*         (CIMC_ARRAY | CIMC_ptr) */
  rb_define_const(klass, "CharsPtrA",   INT2FIX(CIMC_charsptrA)); /*    (CIMC_ARRAY | CIMC_charsptr) */

  // the following are cimcObjectPath key-types synonyms
  // and are valid only when CIMC_keyValue of cimcValueState is set

  rb_define_const(klass, "KeyInteger",  INT2FIX(CIMC_keyInteger)); /*   (CIMC_sint64) */
  rb_define_const(klass, "KeyString",   INT2FIX(CIMC_keyString)); /*   (CIMC_string) */
  rb_define_const(klass, "KeyBoolean",  INT2FIX(CIMC_keyBoolean)); /*   (CIMC_boolean) */
  rb_define_const(klass, "KeyReference",INT2FIX(CIMC_keyRef)); /*       (CIMC_ref) */

  // the following are predicate types only

  rb_define_const(klass, "CharString",      INT2FIX(CIMC_charString)); /*      (CIMC_string) */
  rb_define_const(klass, "IntegerString",   INT2FIX(CIMC_integerString)); /*   (CIMC_string | CIMC_sint64) */
  rb_define_const(klass, "RealString",      INT2FIX(CIMC_realString)); /*      (CIMC_string | CIMC_real64) */
  rb_define_const(klass, "NumericString",   INT2FIX(CIMC_numericString)); /*   (CIMC_string | CIMC_sint64 | CIMC_real64) */
  rb_define_const(klass, "BooleanString",   INT2FIX(CIMC_booleanString)); /*   (CIMC_string | CIMC_boolean) */
  rb_define_const(klass, "DateTimeString",  INT2FIX(CIMC_dateTimeString)); /*  (CIMC_string | CIMC_dateTime) */
  rb_define_const(klass, "ClassNameString", INT2FIX(CIMC_classNameString)); /* (CIMC_string | CIMC_class) */
  rb_define_const(klass, "NameString",      INT2FIX(CIMC_nameString)); /*      (CIMC_string | ((16+10)<<8)) */

}
