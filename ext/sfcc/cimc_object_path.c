
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

  VALUE klass = rb_define_class_under(cimc, "ObjectPath", rb_cObject);
  cSfccCimcObjectPath = klass;

  rb_define_method(klass, "namespace=", set_namespace, 1);
  rb_define_method(klass, "namespace", namespace, 0);
  rb_define_method(klass, "hostname=", set_hostname, 1);
  rb_define_method(klass, "hostname", hostname, 0);
  rb_define_method(klass, "class_name=", set_class_name, 1);
  rb_define_method(klass, "class_name", class_name, 0);
  rb_define_method(klass, "to_s", to_s, 0);
}
