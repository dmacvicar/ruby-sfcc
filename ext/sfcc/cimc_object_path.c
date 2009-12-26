
#include "cimc_object_path.h"

static void
dealloc(CIMCObjectPath *object_path)
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
  CIMCObjectPath *ptr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  ptr->ft->setNameSpace(ptr, StringValuePtr(val));
  return val;
}

/**
 * call-seq:
 *   namespace()
 * Get the namespace component
 */
static VALUE namespace(VALUE self)
{
  CIMCObjectPath *ptr = NULL;
  CIMCString *cimstr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  cimstr = ptr->ft->getNameSpace(ptr, NULL);
  return cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
}

/**
 * call-seq:
 *   hostname=(ns)
 * Set/replace the hostname component
 */
static VALUE set_hostname(VALUE self, VALUE val)
{
  CIMCObjectPath *ptr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  ptr->ft->setHostname(ptr, StringValuePtr(val));
  return val;
}

/**
 * call-seq:
 *   hostname()
 * Get the hostname component
 */
static VALUE hostname(VALUE self)
{
  CIMCObjectPath *ptr = NULL;
  CIMCString *cimstr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  cimstr = ptr->ft->getHostname(ptr, NULL);
  return cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
}

/**
 * call-seq:
 *   class_name=(ns)
 * Set/replace the class name component
 */
static VALUE set_class_name(VALUE self, VALUE val)
{
  CIMCObjectPath *ptr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
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
  CIMCObjectPath *ptr = NULL;
  CIMCString *cimstr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
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
  CIMCObjectPath *ptr = NULL;
  CIMCData data;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  data = sfcc_value_to_cimcdata(value);
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
  CIMCObjectPath *ptr = NULL;
  CIMCStatus status;
  CIMCData data;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  data = ptr->ft->getKey(ptr, StringValuePtr(name), &status);
  if ( !status.rc )
    return sfcc_cimcdata_to_value(data);

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
  CIMCObjectPath *ptr = NULL;
  CIMCStatus status;
  int k=0;
  int num_props=0;
  CIMCString *key_name = NULL;
  CIMCData data;
  Data_Get_Struct(self, CIMCObjectPath, ptr);

  num_props = ptr->ft->getKeyCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getKeyAt(ptr, k, &key_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (key_name ? rb_str_new2(key_name->ft->getCharPtr(key_name, NULL)) : Qnil), sfcc_cimcdata_to_value(data));
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
  CIMCObjectPath *ptr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  return UINT2NUM(ptr->ft->getKeyCount(ptr, NULL));
}

/**
 * call-seq:
 *   to_s
 * Generates a well formed string representation of this ObjectPath
 */
static VALUE to_s(VALUE self)
{
  CIMCObjectPath *ptr = NULL;
  CIMCString *cimstr = NULL;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  cimstr = ptr->ft->toString(ptr, NULL);
  return cimstr ? rb_str_new2(cimstr->ft->getCharPtr(cimstr, NULL)) : Qnil;
}

VALUE
Sfcc_wrap_cimc_object_path(CIMCObjectPath *object_path)
{
  SFCC_INC_REFCOUNT(object_path);
  return Data_Wrap_Struct(cSfccCimcObjectPath, NULL, dealloc, object_path);
}

VALUE cSfccCimcObjectPath;
void init_cimc_object_path()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  /**
   * an object path on the CIM namespace
   */
  VALUE klass = rb_define_class_under(cimc, "ObjectPath", rb_cObject);
  cSfccCimcObjectPath = klass;

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
