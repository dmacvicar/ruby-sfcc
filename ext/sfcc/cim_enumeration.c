
#include "cim_enumeration.h"
#include "cim_object_path.h"

static void
dealloc(CIMCEnumeration *enm)
{
/*  fprintf(stderr, "Sfcc_dealloc_cim_enumeration %p\n", enm); */
  enm->ft->release(enm);
}

/**
 * call-seq:
 *   enumeration.each do |value|
 *      ...
 *   end
 *
 * iterates over all values in the enumeration
 *
 */
static VALUE each(VALUE self)
{
  CIMCStatus status;
  CIMCEnumeration *ptr;
  CIMCArray *ary;
  CIMCData data;
  Data_Get_Struct(self, CIMCEnumeration, ptr);

  /* since getNext() changes the Enumeration, we cannot iterate
     use Array representation instead */
  ary = ptr->ft->toArray(ptr, &status);
  if (!status.rc) {
    CIMCCount count = ary->ft->getSize(ary, NULL);
    CIMCCount idx;
    for (idx = 0; idx < count; ++idx) {
      VALUE value;
      data = ary->ft->getElementAt(ary, idx, NULL);
      value = sfcc_cimdata_to_value(data);
      rb_yield(value);
    }
  }

  sfcc_rb_raise_if_error(status, "Can't iterate enumeration");
  return Qnil;
}


/**
 * call-seq:
 *   enumeration.size -> int
 *
 * returns the size (number of elements) of the enumeration
 *
 */
static VALUE size(VALUE self)
{
  CIMCStatus status;
  CIMCEnumeration *ptr;
  CIMCArray *ary;
  Data_Get_Struct(self, CIMCEnumeration, ptr);

  ary = ptr->ft->toArray(ptr, &status);
  if (!status.rc) {
    CIMCCount count = ary->ft->getSize(ary, NULL);
    return INT2NUM(count);
  }

  sfcc_rb_raise_if_error(status, "Can't get enumeration size");
  return Qnil;
}


/**
 * call-seq:
 *   enumeration.simple_type -> int
 *
 * returns the element type of the enumeration elements
 *
 */
static VALUE simple_type(VALUE self)
{
  CIMCStatus status;
  CIMCEnumeration *ptr;
  CIMCArray *ary;
  Data_Get_Struct(self, CIMCEnumeration, ptr);

  ary = ptr->ft->toArray(ptr, &status);
  if (!status.rc) {
    CIMCType type = ary->ft->getSimpleType(ary, NULL);
    return INT2NUM(type);
  }
  
  sfcc_rb_raise_if_error(status, "Can't get enumeration type");
  return Qnil;
}


VALUE
Sfcc_wrap_cim_enumeration(CIMCEnumeration *enm)
{
  return Data_Wrap_Struct(cSfccCimEnumeration, NULL, dealloc, enm);
}

VALUE cSfccCimEnumeration;
void init_cim_enumeration()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  VALUE klass = rb_define_class_under(cimc, "Enumeration", rb_cObject);
  cSfccCimEnumeration = klass;

  rb_define_method(klass, "each", each, 0);
  rb_define_method(klass, "size", size, 0);
  rb_define_method(klass, "simple_type", simple_type, 0);
  rb_include_module(klass, rb_const_get(rb_cObject, rb_intern("Enumerable")));
}
