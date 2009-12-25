
#include "cimc_instance.h"
#include "cimc_object_path.h"

static void
dealloc(CIMCInstance *instance)
{
  SFCC_DEC_REFCOUNT(instance);
}

/**
 * call-seq:
 *   property(name)
 * Gets a named property value
 */
static VALUE property(VALUE self, VALUE name)
{
  CIMCInstance *ptr = NULL;
  CIMCStatus status;
  CIMCData data;
  Data_Get_Struct(self, CIMCInstance, ptr);
  data = ptr->ft->getProperty(ptr, StringValuePtr(name), &status);
  if ( !status.rc )
    return sfcc_cimcdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", StringValuePtr(name));
  return Qnil;
}

/**
 * call-seq:
 *   instance.each_property do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the property name and
 * its value
 *
 */
static VALUE each_property(VALUE self)
{
  CIMCInstance *ptr = NULL;
  CIMCStatus status;
  int k=0;
  int num_props=0;
  CIMCString *property_name = NULL;
  CIMCData data;
  Data_Get_Struct(self, CIMCInstance, ptr);

  num_props = ptr->ft->getPropertyCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropertyAt(ptr, k, &property_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_name ? rb_str_new2(property_name->ft->getCharPtr(property_name, NULL)) : Qnil), sfcc_cimcdata_to_value(data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve property #%d", k);
      } 
      if (property_name) CMRelease(property_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve property count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   property_count()
 * Gets the number of properties contained in this Instance
 */
static VALUE property_count(VALUE self)
{
  CIMCInstance *ptr = NULL;
  Data_Get_Struct(self, CIMCInstance, ptr);
  return UINT2NUM(ptr->ft->getPropertyCount(ptr, NULL));
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
  CIMCInstance *ptr = NULL;
  CIMCObjectPath *op;
  Data_Get_Struct(self, CIMCInstance, ptr);
  op = ptr->ft->getObjectPath(ptr, NULL);
  return Sfcc_wrap_cimc_object_path(op);
}

VALUE
Sfcc_wrap_cimc_instance(CIMCInstance *instance)
{
  SFCC_INC_REFCOUNT(instance);
  return Data_Wrap_Struct(cSfccCimcInstance, NULL, dealloc, instance);
}

VALUE cSfccCimcInstance;
void init_cimc_instance()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  VALUE klass = rb_define_class_under(cimc, "Instance", rb_cObject);
  cSfccCimcInstance = klass;

  rb_define_method(klass, "property", property, 1);
  rb_define_method(klass, "each_property", each_property, 0);
  rb_define_method(klass, "property_count", property_count, 0);
  rb_define_method(klass, "object_path", object_path, 0);
}
