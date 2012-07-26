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

CIMCType Sfcc_rb_type_to_i(VALUE self)
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
  return INT2FIX(Sfcc_rb_type_to_i(self));
}

char const * Sfcc_cim_type_to_s(CIMCType type) {
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
  char const * s = Sfcc_cim_type_to_s(Sfcc_rb_type_to_i(self));
  if (s) {
    return rb_str_new2(s);
  }
  return Qnil;
}


VALUE Sfcc_wrap_cim_type(CIMCType type)
{
  rb_sfcc_type *rst = (rb_sfcc_type *)malloc(sizeof(rb_sfcc_type));
  if (!rst)
    rb_raise(rb_eNoMemError, "Cannot alloc rb_sfcc_type");
  rst->type = type;
  return Data_Wrap_Struct(cSfccCimType, NULL, dealloc, rst);
}

#define TYPEDEF(name, val) \
    rb_define_const(klass, name, INT2FIX(val));

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

  TYPEDEF("Null"        , CIMC_null);

  TYPEDEF("SIMPLE"      , CIMC_SIMPLE);
  TYPEDEF("Boolean"     , CIMC_boolean);
  TYPEDEF("Char16"      , CIMC_char16);

  TYPEDEF("REAL"        , CIMC_REAL);
  TYPEDEF("Real32"      , CIMC_real32);
  TYPEDEF("Real64"      , CIMC_real64);

  TYPEDEF("UINT"        , CIMC_UINT);
  TYPEDEF("UInt8"       , CIMC_uint8);
  TYPEDEF("UInt16"      , CIMC_uint16);
  TYPEDEF("UInt32"      , CIMC_uint32);
  TYPEDEF("UInt64"      , CIMC_uint64);
  TYPEDEF("SINT"        , CIMC_SINT);
  TYPEDEF("SInt8"       , CIMC_sint8);
  TYPEDEF("SInt16"      , CIMC_sint16);
  TYPEDEF("SInt32"      , CIMC_sint32);
  TYPEDEF("SInt64"      , CIMC_sint64);
  TYPEDEF("INTEGER"     , CIMC_INTEGER);

  TYPEDEF("ENC"         , CIMC_ENC);
  TYPEDEF("Instance"    , CIMC_instance);
  TYPEDEF("Reference"   , CIMC_ref);
  TYPEDEF("Args"        , CIMC_args);
  TYPEDEF("Class"       , CIMC_class);
  TYPEDEF("Filter"      , CIMC_filter);
  TYPEDEF("Enumeration" , CIMC_enumeration);
  TYPEDEF("String"      , CIMC_string);
  TYPEDEF("Chars"       , CIMC_chars);
  TYPEDEF("DateTime"    , CIMC_dateTime);
  TYPEDEF("Ptr"         , CIMC_ptr);
  TYPEDEF("CharsPtr"    , CIMC_charsptr);

  TYPEDEF("ARRAY"      , CIMC_ARRAY); /*    ((1)<<13) */
  TYPEDEF("SIMPLEA"    , CIMC_SIMPLEA); /*  (CIMC_ARRAY | CIMC_SIMPLE) */
  TYPEDEF("BooleanA"   , CIMC_booleanA); /* (CIMC_ARRAY | CIMC_boolean) */
  TYPEDEF("Char16A"    , CIMC_char16A); /*  (CIMC_ARRAY | CIMC_char16) */

  TYPEDEF("REALA"      , CIMC_REALA); /*    (CIMC_ARRAY | CIMC_REAL) */
  TYPEDEF("Real32A"    , CIMC_real32A); /*  (CIMC_ARRAY | CIMC_real32) */
  TYPEDEF("Real64A"    , CIMC_real64A); /*  (CIMC_ARRAY | CIMC_real64) */

  TYPEDEF("UNITA"      , CIMC_UINTA); /*    (CIMC_ARRAY | CIMC_UINT) */
  TYPEDEF("UInt8A"     , CIMC_uint8A); /*   (CIMC_ARRAY | CIMC_uint8) */
  TYPEDEF("UInt16A"    , CIMC_uint16A); /*  (CIMC_ARRAY | CIMC_uint16) */
  TYPEDEF("UInt32A"    , CIMC_uint32A); /*  (CIMC_ARRAY | CIMC_uint32) */
  TYPEDEF("UInt64A"    , CIMC_uint64A); /*  (CIMC_ARRAY | CIMC_uint64) */
  TYPEDEF("SINTA"      , CIMC_SINTA); /*    (CIMC_ARRAY | CIMC_SINT) */
  TYPEDEF("SInt8A"     , CIMC_sint8A); /*   (CIMC_ARRAY | CIMC_sint8) */
  TYPEDEF("SInt16A"    , CIMC_sint16A); /*  (CIMC_ARRAY | CIMC_sint16) */
  TYPEDEF("SInt32A"    , CIMC_sint32A); /*  (CIMC_ARRAY | CIMC_sint32) */
  TYPEDEF("SInt64A"    , CIMC_sint64A); /*  (CIMC_ARRAY | CIMC_sint64) */
  TYPEDEF("INTEGERA"   , CIMC_INTEGERA); /* (CIMC_ARRAY | CIMC_INTEGER) */

  TYPEDEF("ENCA"       , CIMC_ENCA); /*     (CIMC_ARRAY | CIMC_ENC) */
  TYPEDEF("StringA"    , CIMC_stringA); /*  (CIMC_ARRAY | CIMC_string) */
  TYPEDEF("CharsA"     , CIMC_charsA); /*   (CIMC_ARRAY | CIMC_chars) */
  TYPEDEF("DataTimeA"  , CIMC_dateTimeA); /*(CIMC_ARRAY | CIMC_dateTime) */
  TYPEDEF("InstanceA"  , CIMC_instanceA); /*(CIMC_ARRAY | CIMC_instance) */
  TYPEDEF("ReferenceA" , CIMC_refA); /*     (CIMC_ARRAY | CIMC_ref) */
  TYPEDEF("PtrA"       , CIMC_ptrA); /*     (CIMC_ARRAY | CIMC_ptr) */
  TYPEDEF("CharsPtrA"  , CIMC_charsptrA); /*(CIMC_ARRAY | CIMC_charsptr) */

  // the following are cimcObjectPath key-types synonyms
  // and are valid only when CIMC_keyValue of cimcValueState is set

  TYPEDEF("KeyInteger"   , CIMC_keyInteger); /*   (CIMC_sint64) */
  TYPEDEF("KeyString"    , CIMC_keyString); /*    (CIMC_string) */
  TYPEDEF("KeyBoolean"   , CIMC_keyBoolean); /*   (CIMC_boolean) */
  TYPEDEF("KeyReference" , CIMC_keyRef); /*       (CIMC_ref) */

  // the following are predicate types only

  TYPEDEF("CharString"      , CIMC_charString); /*      (CIMC_string) */
  TYPEDEF("IntegerString"   , CIMC_integerString); /*   (CIMC_string | CIMC_sint64) */
  TYPEDEF("RealString"      , CIMC_realString); /*      (CIMC_string | CIMC_real64) */
  TYPEDEF("NumericString"   , CIMC_numericString); /*   (CIMC_string | CIMC_sint64 | CIMC_real64) */
  TYPEDEF("BooleanString"   , CIMC_booleanString); /*   (CIMC_string | CIMC_boolean) */
  TYPEDEF("DateTimeString"  , CIMC_dateTimeString); /*  (CIMC_string | CIMC_dateTime) */
  TYPEDEF("ClassNameString" , CIMC_classNameString); /* (CIMC_string | CIMC_class) */
  TYPEDEF("NameString"      , CIMC_nameString); /*      (CIMC_string | ((16+10)<<8)) */

}
