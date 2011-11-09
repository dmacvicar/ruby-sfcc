#include "cim_string.h"

static void
dealloc(CMPIString *string)
{
  fprintf(stderr, "dealloc(CMPIString %p\n", string);
  SFCC_DEC_REFCOUNT(string);
}

/**
 * call-seq:
 *   to_s
 * Generates a well formed string representation of this String
 */
static VALUE to_s(VALUE self)
{
  CMPIString *ptr = NULL;
  char *str = NULL;
  Data_Get_Struct(self, CMPIString, ptr);
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
  CMPIStatus status;
  CMPIString *newstr = NULL;
  CMPIString *ptr = newCMPIString(to_charptr(value),
                                  &status);
  newstr = ptr->ft->clone(ptr, &status);
  ptr->ft->release(ptr);
  if (!status.rc)
    return Sfcc_wrap_cim_string(newstr);
  sfcc_rb_raise_if_error(status, "Can't create CIM string");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_string(CMPIString *string)
{
  SFCC_INC_REFCOUNT(string);
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
