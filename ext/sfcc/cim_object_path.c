
#include "cim_object_path.h"

static void
mark(struct mark_struct *ms)
{
  fprintf(stderr, "Sfcc_mark_object_path %p, path %p, client %p\n", ms, ms->cmpi_object, (void *)ms->ruby_value);
  rb_gc_mark(ms->ruby_value);
}

static void
dealloc(struct mark_struct *ms)
{
  fprintf(stderr, "Sfcc_dealloc_object_path %p, path %p\n", ms, ms->cmpi_object);
  SFCC_DEC_REFCOUNT(((CMPIObjectPath *)ms->cmpi_object));
  free(ms);
}

/**
 * call-seq:
 *   namespace=(ns)
 *
 * Set/replace the namespace component
 */
static VALUE set_namespace(VALUE self, VALUE val)
{
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIStatus status;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIString *cimstr;
  CMPIStatus status;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
  CMPIStatus status;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIString *cimstr;
  CMPIStatus status;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
  ptr->ft->setClassName(ptr, to_charptr(val));
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIString *cimstr;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIData data;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIStatus status;
  CMPIData data;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIStatus status;
  int k=0;
  int num_props=0;
  CMPIString *key_name = NULL;
  CMPIData data;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;

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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIObjectPath *src;
  CMPIStatus status;

  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  struct mark_struct *obj1, *obj2;
  CMPIObjectPath *ptr;
  CMPIObjectPath *src;
  CMPIStatus status;

  Data_Get_Struct(self, struct mark_struct, obj1);
  Data_Get_Struct(object_path, struct mark_struct, obj2);
  /* FIXME: check type */
  ptr = (CMPIObjectPath *)obj1->cmpi_object;
  src = (CMPIObjectPath *)obj2->cmpi_object;

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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
  data = ptr->ft->getClassQualifier(ptr, to_charptr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
  data = ptr->ft->getPropertyQualifier(ptr, to_charptr(property_name),
                                       to_charptr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
  data = ptr->ft->getMethodQualifier(ptr, to_charptr(method_name),
                                       to_charptr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
  data = ptr->ft->getParameterQualifier(ptr,
                                        to_charptr(method_name),
                                        to_charptr(parameter_name),
                                        to_charptr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

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
  struct mark_struct *obj;
  CMPIObjectPath *ptr;
  CMPIString *cimstr;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIObjectPath *)obj->cmpi_object;
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
  CMPIObjectPath *ptr;
  CMPIObjectPath *newop;
  memset(&status, 0, sizeof(CMPIStatus));

  rb_scan_args(argc, argv, "11", &namespace, &class_name);

  ptr = newCMPIObjectPath(to_charptr(namespace), to_charptr(class_name),
                          &status);

  newop = ptr->ft->clone(ptr, &status);
  ptr->ft->release(ptr);

  if (!status.rc)
    return Sfcc_wrap_cim_object_path(newop,self);
  sfcc_rb_raise_if_error(status, "Can't create object path");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_object_path(CMPIObjectPath *object_path, VALUE client)
{
  struct mark_struct *obj = (struct mark_struct *)malloc(sizeof (struct mark_struct));
  obj->cmpi_object = object_path;
  obj->ruby_value = client;
  rb_gc_mark(client);
  SFCC_INC_REFCOUNT(object_path);
  return Data_Wrap_Struct(cSfccCimObjectPath, mark, dealloc, obj);
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
