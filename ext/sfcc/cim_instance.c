
#include "cim_instance.h"
#include "cim_object_path.h"

static void
dealloc(CMPIInstance *instance)
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
  CMPIInstance *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  Data_Get_Struct(self, CMPIInstance, ptr);
  data = ptr->ft->getProperty(ptr, StringValuePtr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

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
  CMPIInstance *ptr = NULL;
  CMPIStatus status;
  int k=0;
  int num_props=0;
  CMPIString *property_name = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMPIInstance, ptr);

  num_props = ptr->ft->getPropertyCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropertyAt(ptr, k, &property_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_name ? rb_str_new2(property_name->ft->getCharPtr(property_name, NULL)) : Qnil), sfcc_cimdata_to_value(data));
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
  CMPIInstance *ptr = NULL;
  Data_Get_Struct(self, CMPIInstance, ptr);
  return UINT2NUM(ptr->ft->getPropertyCount(ptr, NULL));
}

/**
 * call-seq:
 *   set_property(name, value)
 * Adds/replaces a names property
 */
static VALUE set_property(VALUE self, VALUE name, VALUE value)
{
  CMPIInstance *ptr = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMPIInstance, ptr);
  data = sfcc_value_to_cimdata(value);
  ptr->ft->setProperty(ptr, StringValuePtr(name), &data.value, data.type);

  return value;
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
  CMPIInstance *ptr = NULL;
  CMPIObjectPath *op;
  Data_Get_Struct(self, CMPIInstance, ptr);
  op = ptr->ft->getObjectPath(ptr, NULL);
  return Sfcc_wrap_cim_object_path(op);
}

/**
 * call-seq
 *   new
 * Creates an instance from in +object_path+
 *
 */
static VALUE new(VALUE klass, VALUE object_path)
{
  CMPIStatus status;
  CMPIInstance *ptr;
  CMPIObjectPath *op;

  Data_Get_Struct(object_path, CMPIObjectPath, op);
  ptr = newCMPIInstance(op, &status);

  if (!status.rc)
    return Sfcc_wrap_cim_instance(ptr);
  sfcc_rb_raise_if_error(status, "Can't create instance");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_instance(CMPIInstance *instance)
{
  SFCC_INC_REFCOUNT(instance);
  return Data_Wrap_Struct(cSfccCimInstance, NULL, dealloc, instance);
}

VALUE cSfccCimInstance;
void init_cim_instance()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * an instance of a CIM class
   */
  VALUE klass = rb_define_class_under(cimc, "Instance", rb_cObject);
  cSfccCimInstance = klass;

  rb_define_singleton_method(klass, "new", new, 1);
  rb_define_method(klass, "property", property, 1);
  rb_define_method(klass, "each_property", each_property, 0);
  rb_define_method(klass, "property_count", property_count, 0);
  rb_define_method(klass, "set_property", set_property, 2);
  rb_define_method(klass, "object_path", object_path, 0);
}
