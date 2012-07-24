
#include "cim_class.h"
#include "cim_data.h"

static void
dealloc(CIMCClass *c)
{
/*  fprintf(stderr, "Sfcc_dealloc_cim_class %p\n", c); */
  CIMCRelease(c);
}

/**
 * call-seq:
 *   name() -> String
 *
 * gets the class name
 */
static VALUE class_name(VALUE self)
{
  CIMCClass *cimclass;
  CIMCString *name;
  CIMCStatus status = { 0, NULL };
  Data_Get_Struct(self, CIMCClass, cimclass);
  name = cimclass->ft->getClassName(cimclass, &status);
  if ( !status.rc )
    return CIMSTR_2_RUBYSTR(name);

  sfcc_rb_raise_if_error(status, "Can't retrieve class name");
  return Qnil;
}

/**
 * call-seq:
 *   superclass_name() -> String
 *
 * gets the superclass name
 */
static VALUE superclass_name(VALUE self)
{
  CIMCClass *cimclass;
  const char *classname;
  if (*cimcEnvType == 'X') {
    CIMCStatus status = { CIMC_RC_ERR_NOT_SUPPORTED, NULL };    
    sfcc_rb_raise_if_error(status, "Not supported in XML connection");
    return Qnil;
  }
  Data_Get_Struct(self, CIMCClass, cimclass);
  classname = cimclass->ft->getCharSuperClassName(cimclass);
  if (!classname) {
    rb_raise(rb_eArgError, "Cannot retrieve superclass name");
    return Qnil;
  }
  return rb_str_new2(classname);
}


/**
 * call-seq:
 *   keys() -> Array
 *
 * gets the list of keys
 */
static VALUE keys(VALUE self)
{
  VALUE ret;
  CIMCClass *cimclass;
  CIMCArray *keylist;
  
  if (*cimcEnvType == 'X') {
    CIMCStatus status = { CIMC_RC_ERR_NOT_SUPPORTED, NULL };    
    sfcc_rb_raise_if_error(status, "Not supported in XML connection");
    return Qnil;
  }
  Data_Get_Struct(self, CIMCClass, cimclass);
  keylist = cimclass->ft->getKeyList(cimclass);
  if (!keylist)
    return Qnil;
  
  ret = sfcc_cimcarray_to_rubyarray(keylist, Qnil, false);
  CIMCRelease(keylist);
  return ret;
}


/**
 * call-seq:
 *   association?
 *
 * returns True if the class is an association
 */
static VALUE is_association(VALUE self)
{
  CIMCClass *cimclass;
  CIMCBoolean is;
  if (*cimcEnvType == 'X') {
    CIMCStatus status = { CIMC_RC_ERR_NOT_SUPPORTED, NULL };    
    sfcc_rb_raise_if_error(status, "Not supported in XML connection");
    return Qnil;
  }
  Data_Get_Struct(self, CIMCClass, cimclass);
  is = cimclass->ft->isAssociation(cimclass);
  return is ? Qtrue : Qfalse;
}


/**
 * call-seq:
 *   abstract?
 *
 * returns True if the class is abstract
 */
static VALUE is_abstract(VALUE self)
{
  CIMCClass *cimclass;
  CIMCBoolean is;
  if (*cimcEnvType == 'X') {
    CIMCStatus status = { CIMC_RC_ERR_NOT_SUPPORTED, NULL };    
    sfcc_rb_raise_if_error(status, "Not supported in XML connection");
    return Qnil;
  }
  Data_Get_Struct(self, CIMCClass, cimclass);
  is = cimclass->ft->isAbstract(cimclass);
  return is ? Qtrue : Qfalse;
}


/**
 * call-seq:
 *   indication?
 *
 * returns True if the class is an indication
 */
static VALUE is_indication(VALUE self)
{
  CIMCClass *cimclass;
  CIMCBoolean is;
  if (*cimcEnvType == 'X') {
    CIMCStatus status = { CIMC_RC_ERR_NOT_SUPPORTED, NULL };    
    sfcc_rb_raise_if_error(status, "Not supported in XML connection");
    return Qnil;
  }
  Data_Get_Struct(self, CIMCClass, cimclass);
  is = cimclass->ft->isIndication(cimclass);
  return is ? Qtrue : Qfalse;
}


/**
 * call-seq:
 *   property(name) -> Cim::Data
 *
 * gets a named property 
 */
static VALUE property(VALUE self, VALUE name)
{
  CIMCClass *ptr;
  CIMCStatus status = { 0, NULL };
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClass, ptr);
  data = ptr->ft->getProperty(ptr, to_charptr(name), &status);
  if ( !status.rc )
    return Sfcc_make_rb_cim_data(&data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", to_charptr(name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_property do |name, data|
 *      ...
 *   end
 *
 * enumerates properties yielding the property name and
 * its Cim::Data
 *
 */
static VALUE each_property(VALUE self)
{
  CIMCClass *ptr;
  CIMCStatus status = { 0, NULL };
  int k=0;
  int num_props=0;
  CIMCString *property_name;
  CIMCData data;
  Data_Get_Struct(self, CIMCClass, ptr);

  num_props = ptr->ft->getPropertyCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropertyAt(ptr, k, &property_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_name ? rb_str_intern(rb_str_new2(CIMCGetCharsPtr(property_name, NULL))) : Qnil), Sfcc_make_rb_cim_data(&data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve property #%d", k);
      }
      if (property_name) CIMCRelease(property_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve property count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   property_count() -> Integer
 *
 * Gets the number of properties contained in this class
 */
static VALUE property_count(VALUE self)
{
  CIMCClass *ptr;
  Data_Get_Struct(self, CIMCClass, ptr);
  return UINT2NUM(ptr->ft->getPropertyCount(ptr, NULL));
}

/**
 * call-seq:
 *   qualifier(name) -> Cim::Data
 *
 * gets a named qualifier
 */
static VALUE qualifier(VALUE self, VALUE name)
{
  CIMCClass *ptr;
  CIMCStatus status = { 0, NULL };
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClass, ptr);
  data = ptr->ft->getQualifier(ptr, to_charptr(name), &status);
  if ( !status.rc )
    return Sfcc_make_rb_cim_data(&data);

  sfcc_rb_raise_if_error(status, "Can't retrieve qualifier '%s'", to_charptr(name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_qualifier do |name, data|
 *      ...
 *   end
 *
 * enumerates properties yielding the qualifier name and
 * its Cim::Data
 *
 */
static VALUE each_qualifier(VALUE self)
{
  CIMCClass *ptr;
  CIMCStatus status = { 0, NULL };
  int k=0;
  int num_props=0;
  CIMCString *qualifier_name;
  CIMCData data;
  Data_Get_Struct(self, CIMCClass, ptr);

  num_props = ptr->ft->getQualifierCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getQualifierAt(ptr, k, &qualifier_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (qualifier_name ? rb_str_intern(rb_str_new2(CIMCGetCharsPtr(qualifier_name, NULL))) : Qnil), Sfcc_make_rb_cim_data(&data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve qualifier #%d", k);
      }
      if (qualifier_name) CIMCRelease(qualifier_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve qualifier count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   qualifier_count() -> Integer
 *
 * Gets the number of qualifiers in this class
 */
static VALUE qualifier_count(VALUE self)
{
  CIMCClass *ptr;
  Data_Get_Struct(self, CIMCClass, ptr);
  return UINT2NUM(ptr->ft->getQualifierCount(ptr, NULL));
}

/**
 * call-seq:
 *   property_qualifier(property_name, qualifier_name) -> Cim::Data
 *
 * gets a named property qualifier
 */
static VALUE property_qualifier(VALUE self, VALUE property_name, VALUE qualifier_name)
{
  CIMCClass *ptr;
  CIMCStatus status = { 0, NULL };
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClass, ptr);
  data = ptr->ft->getPropQualifier(ptr, to_charptr(property_name),
                                        to_charptr(qualifier_name), &status);
  if ( !status.rc )
    return Sfcc_make_rb_cim_data(&data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property_qualifier '%s'", to_charptr(qualifier_name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_property_qualifier(property_name) do |name, data|
 *      ...
 *   end
 *
 * enumerates properties yielding the property qualifier name and
 * its Cim::Data
 *
 */
static VALUE each_property_qualifier(VALUE self, VALUE property_name)
{
  CIMCClass *ptr;
  CIMCStatus status = { 0, NULL };
  int k=0;
  int num_props=0;
  CIMCString *property_qualifier_name;
  CIMCData data;
  Data_Get_Struct(self, CIMCClass, ptr);

  num_props = ptr->ft->getPropQualifierCount(ptr, to_charptr(property_name), &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropQualifierAt(ptr, to_charptr(property_name), k, &property_qualifier_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_qualifier_name ? rb_str_intern(rb_str_new2(CIMCGetCharsPtr(property_qualifier_name, NULL))) : Qnil), Sfcc_make_rb_cim_data(&data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier #%d", k);
      }
      if (property_qualifier_name) CIMCRelease(property_qualifier_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   property_qualifier_count(property_name) -> Integer
 *
 * Gets the number of qualifiers contained in this property
 */
static VALUE property_qualifier_count(VALUE self, VALUE property_name)
{
  CIMCClass *ptr;
  Data_Get_Struct(self, CIMCClass, ptr);
  return UINT2NUM(ptr->ft->getPropQualifierCount(ptr, to_charptr(property_name), NULL));
}

VALUE
Sfcc_wrap_cim_class(CIMCClass *cimclass)
{
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
  rb_define_method(klass, "superclass_name", superclass_name, 0);
  rb_define_method(klass, "keys", keys, 0);
  rb_define_method(klass, "association?", is_association, 0);
  rb_define_method(klass, "abstract?", is_abstract, 0);
  rb_define_method(klass, "indication?", is_indication, 0);
}
