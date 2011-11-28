#include "cim_string.h"

static void
dealloc(CIMCString *string)
{
/*  fprintf(stderr, "dealloc(CIMCString %p\n", string); */
  string->ft->release(string);
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

/**
 * call-seq
 *   new
 * Creates a CIM string from a string value
 *
 */
static VALUE new(VALUE klass, VALUE value)
{
  CIMCStatus status;
  CIMCString *ptr = cimcEnv->ft->newString(cimcEnv, to_charptr(value), &status);
  if (!status.rc)
    return Sfcc_wrap_cim_string(ptr);
  sfcc_rb_raise_if_error(status, "Can't create CIM string");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_string(CIMCString *string)
{
  return Data_Wrap_Struct(cSfccCimString, NULL, dealloc, string);
}

VALUE cSfccCimString;
void init_cim_string()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * a string in the CIM environemtn
   */
  VALUE klass = rb_define_class_under(cimc, "String", rb_cObject);
  cSfccCimString = klass;

  rb_define_singleton_method(klass, "new", new, 1);
  rb_define_method(klass, "to_s", to_s, 0);
}
