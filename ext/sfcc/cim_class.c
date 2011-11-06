
#include "cim_class.h"

static void
dealloc(CMPIConstClass *cimclass)
{
  SFCC_DEC_REFCOUNT(cimclass);
}

/**
 * call-seq:
 *   name()
 *
 * gets the class name
 */
static VALUE class_name(VALUE self)
{
  CMPIConstClass *cimclass = NULL;
  CMPIString *classname;
  Data_Get_Struct(self, CMPIConstClass, cimclass);
  classname = cimclass->ft->getClassName(cimclass, NULL);
  return rb_str_new2(classname->ft->getCharPtr(classname, NULL));
}

/**
 * call-seq:
 *   property(name)
 *
 * gets a named property value
 */
static VALUE property(VALUE self, VALUE name)
{
  CMPIConstClass *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMPIConstClass, ptr);
  data = ptr->ft->getProperty(ptr, to_charptr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", to_charptr(name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_property do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the property name and
 * its value
 *
 */
static VALUE each_property(VALUE self)
{
  CMPIConstClass *ptr = NULL;
  CMPIStatus status;
  int k=0;
  int num_props=0;
  CMPIString *property_name = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMPIConstClass, ptr);

  num_props = ptr->ft->getPropertyCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropertyAt(ptr, k, &property_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_name ? rb_str_intern(rb_str_new2(property_name->ft->getCharPtr(property_name, NULL))) : Qnil), sfcc_cimdata_to_value(data));
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
 *
 * Gets the number of properties contained in this class
 */
static VALUE property_count(VALUE self)
{
  CMPIConstClass *ptr = NULL;
  Data_Get_Struct(self, CMPIConstClass, ptr);
  return UINT2NUM(ptr->ft->getPropertyCount(ptr, NULL));
}

/**
 * call-seq:
 *   qualifier(name)
 *
 * gets a named qualifier value
 */
static VALUE qualifier(VALUE self, VALUE name)
{
  CMPIConstClass *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMPIConstClass, ptr);
  data = ptr->ft->getQualifier(ptr, to_charptr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve qualifier '%s'", to_charptr(name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_qualifier do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the qualifier name and
 * its value
 *
 */
static VALUE each_qualifier(VALUE self)
{
  CMPIConstClass *ptr = NULL;
  CMPIStatus status;
  int k=0;
  int num_props=0;
  CMPIString *qualifier_name = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMPIConstClass, ptr);

  num_props = ptr->ft->getQualifierCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getQualifierAt(ptr, k, &qualifier_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (qualifier_name ? rb_str_intern(rb_str_new2(qualifier_name->ft->getCharPtr(qualifier_name, NULL))) : Qnil), sfcc_cimdata_to_value(data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve qualifier #%d", k);
      } 
      if (qualifier_name) CMRelease(qualifier_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve qualifier count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   qualifier_count()
 *
 * Gets the number of qualifiers in this class
 */
static VALUE qualifier_count(VALUE self)
{
  CMPIConstClass *ptr = NULL;
  Data_Get_Struct(self, CMPIConstClass, ptr);
  return UINT2NUM(ptr->ft->getQualifierCount(ptr, NULL));
}

/**
 * call-seq:
 *   property_qualifier(property_name, qualifier_name)
 *
 * gets a named property qualifier value
 */
static VALUE property_qualifier(VALUE self, VALUE property_name, VALUE qualifier_name)
{
  CMPIConstClass *ptr = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMPIConstClass, ptr);
  data = ptr->ft->getPropertyQualifier(ptr, to_charptr(property_name),
                                        to_charptr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property_qualifier '%s'", to_charptr(qualifier_name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_property_qualifier(property_name) do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the property qualifier name and
 * its value
 *
 */
static VALUE each_property_qualifier(VALUE self, VALUE property_name)
{
  CMPIConstClass *ptr = NULL;
  CMPIStatus status;
  int k=0;
  int num_props=0;
  CMPIString *property_qualifier_name = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMPIConstClass, ptr);

  num_props = ptr->ft->getPropertyQualifierCount(ptr, to_charptr(property_name), &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropertyQualifierAt(ptr, to_charptr(property_name), k, &property_qualifier_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_qualifier_name ? rb_str_intern(rb_str_new2(property_qualifier_name->ft->getCharPtr(property_qualifier_name, NULL))) : Qnil), sfcc_cimdata_to_value(data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier #%d", k);
      } 
      if (property_qualifier_name) CMRelease(property_qualifier_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   property_qualifier_count(property_name)
 *
 * Gets the number of qualifiers contained in this property
 */
static VALUE property_qualifier_count(VALUE self, VALUE property_name)
{
  CMPIConstClass *ptr = NULL;
  Data_Get_Struct(self, CMPIConstClass, ptr);
  return UINT2NUM(ptr->ft->getPropertyQualifierCount(ptr, to_charptr(property_name), NULL));
}

VALUE
Sfcc_wrap_cim_class(CMPIConstClass *cimclass)
{
  SFCC_INC_REFCOUNT(cimclass);
  return Data_Wrap_Struct(cSfccCimClass, NULL, dealloc, cimclass);
}

VALUE cSfccCimClass;
void init_cim_class()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * class from the CIM schema
   */
  VALUE klass = rb_define_class_under(cimc, "Class", rb_cObject);
  cSfccCimClass = klass;

  rb_define_method(klass, "class_name", class_name, 0);
  rb_define_method(klass, "property", property, 1);
  rb_define_method(klass, "each_property", each_property, 0);
  rb_define_method(klass, "property_count", property_count, 0);
  rb_define_method(klass, "qualifier", qualifier, 1);
  rb_define_method(klass, "each_qualifier", each_qualifier, 0);
  rb_define_method(klass, "qualifier_count", qualifier_count, 0);
  rb_define_method(klass, "property_qualifier", property_qualifier, 2);
  rb_define_method(klass, "each_property_qualifier", each_property_qualifier, 1);
  rb_define_method(klass, "property_qualifier_count", property_qualifier_count, 1);
}
