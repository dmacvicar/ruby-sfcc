
#include "cim_class.h"

static void
dealloc(CMPIConstClass *cimclass)
{
  SFCC_DEC_REFCOUNT(cimclass);
}

static VALUE class_name(VALUE self)
{
  CMPIConstClass *cimclass = NULL;
  CMPIString *classname;
  Data_Get_Struct(self, CMPIConstClass, cimclass);
  classname = cimclass->ft->getClassName(cimclass, NULL);
  return rb_str_new2(classname->ft->getCharPtr(classname, NULL));
}

VALUE
Sfcc_wrap_cim_class(CMPIConstClass *cimclass)
{
  SFCC_INC_REFCOUNT(cimclass);
  return Data_Wrap_Struct(cSfccCimClass, NULL, dealloc, cimclass);
}

VALUE cSfccCimClass;
void init_cim_class()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * class from the CIM schema
   */
  VALUE klass = rb_define_class_under(cimc, "Class", rb_cObject);
  cSfccCimClass = klass;

  rb_define_method(klass, "class_name", class_name, 0);
}
