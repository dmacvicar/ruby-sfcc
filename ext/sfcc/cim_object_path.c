
#include "cim_object_path.h"

static void
dealloc(CMPIObjectPath *object_path)
{
  //SFCC_DEC_REFCOUNT(object_path);
}

/**
 * call-seq:
 *   namespace=(ns)
 *
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
 *
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
    return CIMSTR_2_RUBYSTR(cimstr);
  sfcc_rb_raise_if_error(status, "Can't get namespace");
  return Qnil;
}

/**
 * call-seq:
 *   hostname=(ns)
 *
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
 *
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
    return CIMSTR_2_RUBYSTR(cimstr);
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
 *
 * Get the class name component
 */
static VALUE class_name(VALUE self)
{
  CMPIObjectPath *ptr = NULL;
  CMPIString *cimstr = NULL;
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  cimstr = ptr->ft->getClassName(ptr, NULL);
  return CIMSTR_2_RUBYSTR(cimstr);
}

/**
 * call-seq:
 *   add_key(name, value)
 *
 * adds a named key value
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
 *
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
        rb_yield_values(2, rb_str_intern(CIMSTR_2_RUBYSTR(key_name)), sfcc_cimdata_to_value(data));
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
 *
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
 *   set_namespace_from(object_path)
 *
 * Set/replace the namespace and classname components
 * from +object_path+
 */
static VALUE set_namespace_from(VALUE self, VALUE object_path)
{
  CMPIObjectPath *ptr = NULL;
  CMPIObjectPath *src = NULL;
  CMPIStatus status;

  Data_Get_Struct(self, CMPIObjectPath, ptr);
  Data_Get_Struct(object_path, CMPIObjectPath, src);

  status = ptr->ft->setNameSpaceFromObjectPath(ptr, src);

  if (!status.rc)
    return self;

  sfcc_rb_raise_if_error(status, "Can't set namespace");
  return Qnil;
}

/**
 * call-seq:
 *   set_host_and_namespace_from(object_path)
 *
 * Set/replace the hostname namespace and classname components
 * from +object_path+
 */
static VALUE set_host_and_namespace_from(VALUE self, VALUE object_path)
{
  CMPIObjectPath *ptr = NULL;
  CMPIObjectPath *src = NULL;
  CMPIStatus status;

  Data_Get_Struct(self, CMPIObjectPath, ptr);
  Data_Get_Struct(object_path, CMPIObjectPath, src);

  status = ptr->ft->setHostAndNameSpaceFromObjectPath(ptr, src);

  if (!status.rc)
    return self;

  sfcc_rb_raise_if_error(status, "Can't set namespace and hostname");
  return Qnil;
}

/**
 * call-seq:
 *   class_qualifier(qualifier_name)
 *
 * Get class qualifier value
 */
static VALUE class_qualifier(VALUE self, VALUE qualifier_name)
{
  CMPIObjectPath *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  data = ptr->ft->getClassQualifier(ptr, StringValuePtr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve class qualifier '%s'", StringValuePtr(qualifier_name));
  return Qnil;
}

/**
 * call-seq:
 *   property_qualifier(property_name, qualifier_name)
 *
 * Get property qualifier value
 */
static VALUE property_qualifier(VALUE self, VALUE property_name, VALUE qualifier_name)
{
  CMPIObjectPath *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  data = ptr->ft->getPropertyQualifier(ptr, StringValuePtr(property_name),
                                       StringValuePtr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier '%s' for property '%s'", StringValuePtr(qualifier_name), StringValuePtr(property_name));
  return Qnil;
}

/**
 * call-seq:
 *   method_qualifier(method_name, qualifier_name)
 *
 * Get method qualifier value
 */
static VALUE method_qualifier(VALUE self, VALUE method_name, VALUE qualifier_name)
{
  CMPIObjectPath *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  data = ptr->ft->getMethodQualifier(ptr, StringValuePtr(method_name),
                                       StringValuePtr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve method qualifier '%s' for method '%s'", StringValuePtr(qualifier_name), StringValuePtr(method_name));
  return Qnil;
}

/**
 * call-seq:
 *   parameter_qualifier(parameter_name, qualifier_name)
 *
 * Get parameter qualifier value
 */
static VALUE parameter_qualifier(VALUE self,
                                 VALUE method_name, 
                                 VALUE parameter_name,
                                 VALUE qualifier_name)
{
  CMPIObjectPath *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMPIObjectPath, ptr);
  data = ptr->ft->getParameterQualifier(ptr,
                                        StringValuePtr(method_name),
                                        StringValuePtr(parameter_name),
                                        StringValuePtr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve parameter qualifier '%s' for '%s'/'%s'", StringValuePtr(qualifier_name), StringValuePtr(method_name), StringValuePtr(parameter_name));
  return Qnil;
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
  return CIMSTR_2_RUBYSTR(cimstr);
}

/**
 * call-seq:
 *   new()
 *
 * Creates an object path from +namespace+ and +class_name+
 *
 */
static VALUE new(int argc, VALUE *argv, VALUE self)
{
  VALUE namespace;
  VALUE class_name;

  CMPIStatus status;
  CMPIObjectPath *ptr = NULL;
  CMPIObjectPath *newop = NULL;
  memset(&status, 0, sizeof(CMPIStatus));

  rb_scan_args(argc, argv, "11", &namespace, &class_name);

  ptr = newCMPIObjectPath(NIL_P(namespace) ? NULL : StringValuePtr(namespace),
                          NIL_P(class_name) ? NULL : StringValuePtr(class_name),
                          &status);

  newop = ptr->ft->clone(ptr, &status);
  ptr->ft->release(ptr);

  if (!status.rc)
    return Sfcc_wrap_cim_object_path(newop);
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

  rb_define_singleton_method(klass, "new", new, -1);
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
  rb_define_method(klass, "set_namespace_from", set_namespace_from, 1);
  rb_define_method(klass, "set_host_and_namespace_from", set_host_and_namespace_from, 1);
  rb_define_method(klass, "class_qualifier", class_qualifier, 1);
  rb_define_method(klass, "property_qualifier", property_qualifier, 2);
  rb_define_method(klass, "method_qualifier", method_qualifier, 2);
  rb_define_method(klass, "parameter_qualifier", parameter_qualifier, 3);
  rb_define_method(klass, "to_s", to_s, 0);
}
