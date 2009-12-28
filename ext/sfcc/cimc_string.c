
#include "cimc_string.h"

static void
dealloc(CIMCString *string)
{
  SFCC_DEC_REFCOUNT(string);
}

/**
 * call-seq:
 *   to_s
 * Generates a well formed string representation of this String
 */
static VALUE to_s(VALUE self)
{
  CIMCString *ptr = NULL;
  char *str = NULL;
  Data_Get_Struct(self, CIMCString, ptr);
  str = ptr->ft->getCharPtr(ptr, NULL);
  return str ? rb_str_new2(str) : Qnil;
}

VALUE
Sfcc_wrap_cimc_string(CIMCString *string)
{
  SFCC_INC_REFCOUNT(string);
  return Data_Wrap_Struct(cSfccCimcString, NULL, dealloc, string);
}

VALUE cSfccCimcString;
void init_cimc_string()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  /**
   * a string in the CIM environemtn
   */
  VALUE klass = rb_define_class_under(cimc, "String", rb_cObject);
  cSfccCimcString = klass;

  rb_define_method(klass, "to_s", to_s, 0);
}
