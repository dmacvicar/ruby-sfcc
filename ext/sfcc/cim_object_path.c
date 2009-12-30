
#include "cim_object_path.h"

static void
dealloc(CMPIObjectPath *object_path)
{
  SFCC_DEC_REFCOUNT(object_path);
}

/**
 * call-seq:
 *   namespace=(ns)
 * Set/replace the namespace component
 */
static VALUE set_namespace(VALUE self, VALUE val)
{
  CMPIObjectPath *ptr = NULL;
  CMPIStatus status;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  status = ptr->ft->setNameSpace(ptr, StringValuePtr(val));
  if (!status.rc)
    return val;
  sfcc_rb_raise_if_error(status, "Can't set namespace");
  return Qnil;
}

/**
 * call-seq:
 *   namespace()
 * Get the namespace component
 */
static VALUE namespace(VALUE self)
{
  CMPIObjectPath *ptr = NULL;
  CMPIString *cimstr = NULL;
  CMPIStatus status;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  cimstr = ptr->ft->getNameSpace(ptr, &status);
  if (!status.rc)
    return cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
  sfcc_rb_raise_if_error(status, "Can't get namespace");
  return Qnil;
}

/**
 * call-seq:
 *   hostname=(ns)
 * Set/replace the hostname component
 */
static VALUE set_hostname(VALUE self, VALUE val)
{
  CMPIObjectPath *ptr = NULL;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  CMPIStatus status;
  status = ptr->ft->setHostname(ptr, StringValuePtr(val));
  if (!status.rc)
    return val;
  sfcc_rb_raise_if_error(status, "Can't set hostname");
  return Qnil;
}

/**
 * call-seq:
 *   hostname()
 * Get the hostname component
 */
static VALUE hostname(VALUE self)
{
  CMPIObjectPath *ptr = NULL;
  CMPIString *cimstr = NULL;
  CMPIStatus status;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  cimstr = ptr->ft->getHostname(ptr, &status);
  if (!status.rc)
    return cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
  sfcc_rb_raise_if_error(status, "Can't get hostname");
  return Qnil;
}

/**
 * call-seq:
 *   class_name=(ns)
 * Set/replace the class name component
 */
static VALUE set_class_name(VALUE self, VALUE val)
{
  CMPIObjectPath *ptr = NULL;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  ptr->ft->setClassName(ptr, StringValuePtr(val));
  return val;
}

/**
 * call-seq:
 *   class_name()
 * Get the class name component
 */
static VALUE class_name(VALUE self)
{
  CMPIObjectPath *ptr = NULL;
  CMPIString *cimstr = NULL;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  cimstr = ptr->ft->getClassName(ptr, NULL);
  return cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
}

/**
 * call-seq:
 *   add_key(name, value)
 * Gets a named key value
 */
static VALUE add_key(VALUE self, VALUE name, VALUE value)
{
  CMPIObjectPath *ptr = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  data = sfcc_value_to_cimdata(value);
  ptr->ft->addKey(ptr, StringValuePtr(name), &data.value, data.type);
  return value;
}

/**
 * call-seq:
 *   key(name)
 * Gets a named key value
 */
static VALUE key(VALUE self, VALUE name)
{
  CMPIObjectPath *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  data = ptr->ft->getKey(ptr, StringValuePtr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve key '%s'", StringValuePtr(name));
  return Qnil;
}

/**
 * call-seq:
 *   ObjectPath.each_key do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the key name and
 * its value
 *
 */
static VALUE each_key(VALUE self)
{
  CMPIObjectPath *ptr = NULL;
  CMPIStatus status;
  int k=0;
  int num_props=0;
  CMPIString *key_name = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMPIObjectPath, ptr);

  num_props = ptr->ft->getKeyCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getKeyAt(ptr, k, &key_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (key_name ? rb_str_new2(key_name->ft->getCharPtr(key_name, NULL)) : Qnil), sfcc_cimdata_to_value(data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve key #%d", k);
      } 
      if (key_name) CMRelease(key_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve key count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   key_count()
 * Gets the number of properties contained in this ObjectPath
 */
static VALUE key_count(VALUE self)
{
  CMPIObjectPath *ptr = NULL;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  return UINT2NUM(ptr->ft->getKeyCount(ptr, NULL));
}

/**
 * call-seq:
 *   to_s
 * Generates a well formed string representation of this ObjectPath
 */
static VALUE to_s(VALUE self)
{
  CMPIObjectPath *ptr = NULL;
  CMPIString *cimstr = NULL;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  cimstr = ptr->ft->toString(ptr, NULL);
  return cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
}

/**
 * call-seq
 *   new
 * Creates an object path from +namespace+ and +class_name+
 *
 */
static VALUE new(VALUE klass, VALUE namespace, VALUE class_name)
{
  CMPIStatus status;
  CMPIObjectPath *ptr = newCMPIObjectPath(StringValuePtr(namespace),
                                          StringValuePtr(class_name),
                                          &status);
  if (!status.rc)
    return Sfcc_wrap_cim_object_path(ptr);
  sfcc_rb_raise_if_error(status, "Can't create object path");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_object_path(CMPIObjectPath *object_path)
{
  SFCC_INC_REFCOUNT(object_path);
  return Data_Wrap_Struct(cSfccCimObjectPath, NULL, dealloc, object_path);
}

VALUE cSfccCimObjectPath;
void init_cim_object_path()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * an object path on the CIM namespace
   */
  VALUE klass = rb_define_class_under(cimc, "ObjectPath", rb_cObject);
  cSfccCimObjectPath = klass;

  rb_define_singleton_method(klass, "new", new, 2);
  rb_define_method(klass, "namespace=", set_namespace, 1);
  rb_define_method(klass, "namespace", namespace, 0);
  rb_define_method(klass, "hostname=", set_hostname, 1);
  rb_define_method(klass, "hostname", hostname, 0);
  rb_define_method(klass, "class_name=", set_class_name, 1);
  rb_define_method(klass, "class_name", class_name, 0);
  rb_define_method(klass, "add_key", add_key, 2);
  rb_define_method(klass, "key", key, 1);
  rb_define_method(klass, "each_key", each_key, 0);
  rb_define_method(klass, "key_count", key_count, 0);
  rb_define_method(klass, "to_s", to_s, 0);
}
