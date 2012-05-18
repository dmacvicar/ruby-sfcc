
#include "cim_object_path.h"

static void
dealloc(CIMCObjectPath *op)
{
/*  fprintf(stderr, "Sfcc_dealloc_object_path %p\n", op); */
  op->ft->release(op);
}

/**
 * call-seq:
 *   namespace=(ns)
 *
 * Set/replace the namespace component
 */
static VALUE set_namespace(VALUE self, VALUE val)
{
  CIMCObjectPath *ptr;
  CIMCStatus status;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  status = ptr->ft->setNameSpace(ptr, to_charptr(val));
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
  CIMCObjectPath *ptr;
  CIMCString *cimstr;
  CIMCStatus status;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
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
  CIMCObjectPath *ptr;
  CIMCStatus status;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  status = ptr->ft->setHostname(ptr, to_charptr(val));
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
  CIMCObjectPath *ptr;
  CIMCString *cimstr;
  CIMCStatus status;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  cimstr = ptr->ft->getHostname(ptr, &status);
  if (!status.rc)
    return CIMSTR_2_RUBYSTR(cimstr);
  sfcc_rb_raise_if_error(status, "Can't get hostname");
  return Qnil;
}

/**
 * call-seq:
 *   classname=(ns)
 * Set/replace the class name component
 */
static VALUE set_classname(VALUE self, VALUE val)
{
  CIMCObjectPath *ptr;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  ptr->ft->setClassName(ptr, to_charptr(val));
  return val;
}

/**
 * call-seq:
 *   classname()
 *
 * Get the class name component
 */
static VALUE classname(VALUE self)
{
  CIMCObjectPath *ptr;
  CIMCString *cimstr;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
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
  CIMCObjectPath *ptr;
  CIMCData data;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  data = sfcc_value_to_cimdata(value);
  ptr->ft->addKey(ptr, to_charptr(name), &data.value, data.type);
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
  CIMCObjectPath *ptr;
  CIMCStatus status;
  CIMCData data;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  data = ptr->ft->getKey(ptr, to_charptr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve key '%s'", to_charptr(name));
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
  CIMCObjectPath *ptr;
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
  CIMCObjectPath *ptr;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
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
  CIMCObjectPath *ptr;
  CIMCObjectPath *src;
  CIMCStatus status;

  Data_Get_Struct(self, CIMCObjectPath, ptr);
  Data_Get_Struct(object_path, CIMCObjectPath, src);

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
  CIMCObjectPath *ptr;
  CIMCObjectPath *src;
  CIMCStatus status;

  Data_Get_Struct(self, CIMCObjectPath, ptr);
  Data_Get_Struct(object_path, CIMCObjectPath, src);

  if (ptr->ft->setHostAndNameSpaceFromObjectPath) { /* might be missing in sfcc/backend/cimxml/objectpath.c */
    status = ptr->ft->setHostAndNameSpaceFromObjectPath(ptr, src);
  }

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
  CIMCObjectPath *ptr;
  CIMCStatus status;
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  if (ptr->ft->getClassQualifier) { /* might be missing in sfcc/backend/cimxml/objectpath.c */
    data = ptr->ft->getClassQualifier(ptr, to_charptr(qualifier_name), &status);
    if ( !status.rc )
      return sfcc_cimdata_to_value(data);
  }
  else {
    status.rc = CMPI_RC_ERR_NOT_SUPPORTED;
  }
  sfcc_rb_raise_if_error(status, "Can't retrieve class qualifier '%s'", to_charptr(qualifier_name));
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
  CIMCObjectPath *ptr;
  CIMCStatus status;
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  if (ptr->ft->getPropertyQualifier) { /* might be missing in sfcc/backend/cimxml/objectpath.c */
    data = ptr->ft->getPropertyQualifier(ptr, to_charptr(property_name),
                                         to_charptr(qualifier_name), &status);
    if ( !status.rc )
      return sfcc_cimdata_to_value(data);
  }
  else {
    status.rc = CMPI_RC_ERR_NOT_SUPPORTED;
  }
  sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier '%s' for property '%s'", to_charptr(qualifier_name), to_charptr(property_name));
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
  CIMCObjectPath *ptr;
  CIMCStatus status;
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  if (ptr->ft->getMethodQualifier) { /* might be missing in sfcc/backend/cimxml/objectpath.c */
    data = ptr->ft->getMethodQualifier(ptr, to_charptr(method_name),
                                       to_charptr(qualifier_name), &status);
    if ( !status.rc )
      return sfcc_cimdata_to_value(data);
  }
  else {
    status.rc = CMPI_RC_ERR_NOT_SUPPORTED;
  }
  sfcc_rb_raise_if_error(status, "Can't retrieve method qualifier '%s' for method '%s'", to_charptr(qualifier_name), to_charptr(method_name));
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
  CIMCObjectPath *ptr;
  CIMCStatus status;
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  if (ptr->ft->getParameterQualifier) { /* might be missing in sfcc/backend/cimxml/objectpath.c */
    data = ptr->ft->getParameterQualifier(ptr,
                                          to_charptr(method_name),
                                          to_charptr(parameter_name),
                                          to_charptr(qualifier_name), &status);
    if ( !status.rc )
      return sfcc_cimdata_to_value(data);
  }
  else {
    status.rc = CMPI_RC_ERR_NOT_SUPPORTED;
  }
  sfcc_rb_raise_if_error(status, "Can't retrieve parameter qualifier '%s' for '%s'/'%s'", to_charptr(qualifier_name), to_charptr(method_name), to_charptr(parameter_name));
  return Qnil;
}

/**
 * call-seq:
 *   to_s
 * Generates a well formed string representation of this ObjectPath
 */
static VALUE to_s(VALUE self)
{
  VALUE ret;
  CIMCObjectPath *ptr;
  CIMCString *cimstr;
  Data_Get_Struct(self, CIMCObjectPath, ptr);
  cimstr = ptr->ft->toString(ptr, NULL);
  ret = CIMSTR_2_RUBYSTR(cimstr);
  CMRelease(cimstr);
  return ret;
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

  CIMCStatus status;
  CIMCObjectPath *ptr;
  memset(&status, 0, sizeof(CIMCStatus));

  rb_scan_args(argc, argv, "11", &namespace, &class_name);

  ptr = cimcEnv->ft->newObjectPath(cimcEnv, to_charptr(namespace), to_charptr(class_name), &status);

  if (!status.rc)
    return Sfcc_wrap_cim_object_path(ptr);
  sfcc_rb_raise_if_error(status, "Can't create object path");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_object_path(CIMCObjectPath *object_path)
{
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
  rb_define_method(klass, "classname=", set_classname, 1);
  rb_define_method(klass, "classname", classname, 0);
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
