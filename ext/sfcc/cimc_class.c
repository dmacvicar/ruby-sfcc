
#include "cimc_class.h"

static void
dealloc(CIMCClass *cimclass)
{
  cimclass->ft->release(cimclass);
}

static VALUE class_name(VALUE self)
{
  CIMCClass *cimclass = NULL;
  CIMCString *classname;
  Data_Get_Struct(self, CIMCClass, cimclass);
  classname = cimclass->ft->getClassName(cimclass, NULL);
  return rb_str_new2(classname->ft->getCharPtr(classname, NULL));
}

VALUE
Sfcc_wrap_cimc_class(CIMCClass *cimclass)
{
  return Data_Wrap_Struct(cSfccCimcClass, NULL, dealloc, cimclass);
}

VALUE cSfccCimcClass;
void init_cimc_class()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  VALUE klass = rb_define_class_under(cimc, "Class", rb_cObject);
  cSfccCimcClass = klass;

  rb_define_method(klass, "class_name", class_name, 0);
}
