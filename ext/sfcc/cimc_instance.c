
#include "cimc_instance.h"
#include "cimc_object_path.h"

static void
dealloc(CIMCInstance *instance)
{
  instance->ft->release(instance);
}

/**
 * call-seq:
 *  object_path()
 *
 * Generates an ObjectPath out of the nameSpace, classname and
 * key propeties of this Instance.
 */
static VALUE object_path(VALUE self)
{
  CIMCInstance *instance = NULL;
  CIMCObjectPath *op;
  Data_Get_Struct(self, CIMCInstance, instance);
  op = instance->ft->getObjectPath(instance, NULL);
  return Sfcc_wrap_cimc_object_path(op);
}

VALUE
Sfcc_wrap_cimc_instance(CIMCInstance *instance)
{
  return Data_Wrap_Struct(cSfccCimcInstance, NULL, dealloc, instance);
}

VALUE cSfccCimcInstance;
void init_cimc_instance()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  VALUE klass = rb_define_class_under(cimc, "Instance", rb_cObject);
  cSfccCimcInstance = klass;

  rb_define_method(klass, "object_path", object_path, 0);
}
