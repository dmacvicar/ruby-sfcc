
#include "cimc_object_path.h"

static void
dealloc(CIMCObjectPath *object_path)
{
  object_path->ft->release(object_path);
}

VALUE
Sfcc_wrap_cimc_object_path(CIMCObjectPath *object_path)
{
  return Data_Wrap_Struct(cSfccCimcObjectPath, NULL, dealloc, object_path);
}

VALUE cSfccCimcObjectPath;
void init_cimc_object_path()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  VALUE klass = rb_define_class_under(cimc, "ObjectPath", rb_cObject);
  cSfccCimcObjectPath = klass;
}
