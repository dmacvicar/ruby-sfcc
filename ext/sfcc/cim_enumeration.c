
#include "cim_enumeration.h"
#include "cim_object_path.h"
#include "cim_client.h"

static void
dealloc(rb_sfcc_enumeration *rse)
{
/*  fprintf(stderr, "Sfcc_dealloc_cim_enumeration %p\n", enm); */
  rse->enm->ft->release(rse->enm);
  free(rse);
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
  rb_sfcc_enumeration *rse;
  CIMCEnumeration *ptr;
  CIMCArray *ary;
  CIMCData data;
  Data_Get_Struct(self, rb_sfcc_enumeration, rse);
  ptr = rse->enm;

  /* since getNext() changes the Enumeration, we cannot iterate
     use Array representation instead */
  ary = ptr->ft->toArray(ptr, &status);
  if (!status.rc) {
    CIMCCount count = ary->ft->getSize(ary, NULL);
    CIMCCount idx;
    for (idx = 0; idx < count; ++idx) {
      VALUE value;
      data = ary->ft->getElementAt(ary, idx, NULL);
      value = sfcc_cimdata_to_value(&data, rse->client);
      rb_yield(value);
    }
    return Qnil;
  }

  sfcc_rb_raise_if_error(status, "Can't iterate enumeration");
  return Qnil;
}


/**
 * call-seq:
 *   enumeration.to_a -> Array
 *
 * returns an Array representation of the enumeration
 *
 */
static VALUE to_a(VALUE self)
{
  CIMCStatus status;
  rb_sfcc_enumeration *rse;
  CIMCEnumeration *ptr;
  CIMCArray *ary;
  Data_Get_Struct(self, rb_sfcc_enumeration, rse);
  ptr = rse->enm;

  ary = ptr->ft->toArray(ptr, &status);
  if (!status.rc) {
    CIMCData data;
    CIMCCount count = ary->ft->getSize(ary, NULL);
    VALUE array = rb_ary_new2(count);
    CIMCCount idx;
    for (idx = 0; idx < count; ++idx) {
      VALUE value;
      data = ary->ft->getElementAt(ary, idx, NULL);
      value = sfcc_cimdata_to_value(&data, rse->client);
      rb_ary_store(array, idx, value);
    }
    return array;
  }

  sfcc_rb_raise_if_error(status, "Can't convert enumeration to Array");
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
  rb_sfcc_enumeration *rse;
  CIMCEnumeration *ptr;
  CIMCArray *ary;
  Data_Get_Struct(self, rb_sfcc_enumeration, rse);
  ptr = rse->enm;

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
  rb_sfcc_enumeration *rse;
  CIMCEnumeration *ptr;
  CIMCArray *ary;
  Data_Get_Struct(self, rb_sfcc_enumeration, rse);
  ptr = rse->enm;

  ary = ptr->ft->toArray(ptr, &status);
  if (!status.rc) {
    CIMCType type = ary->ft->getSimpleType(ary, NULL);
    return INT2NUM(type);
  }
  
  sfcc_rb_raise_if_error(status, "Can't get enumeration type");
  return Qnil;
}


/**
 * call-seq:
 *   enumeration.client -> Client
 *
 * returns the client associated to the enumeration
 *
 */
static VALUE client(VALUE self)
{
  rb_sfcc_enumeration *rse;

  Data_Get_Struct(self, rb_sfcc_enumeration, rse);
  return Sfcc_wrap_cim_client(rse->client);
}


VALUE
Sfcc_wrap_cim_enumeration(CIMCEnumeration *enm, CIMCClient *client)
{
  rb_sfcc_enumeration *rse = (rb_sfcc_enumeration *)malloc(sizeof(rb_sfcc_enumeration));
  if (!rse)
    rb_raise(rb_eNoMemError, "Cannot alloc rb_sfcc_enumeration");

  rse->enm = enm;
  rse->client = client;

  return Data_Wrap_Struct(cSfccCimEnumeration, NULL, dealloc, rse);
}


VALUE cSfccCimEnumeration;
void init_cim_enumeration()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  VALUE klass = rb_define_class_under(cimc, "Enumeration", rb_cObject);
  cSfccCimEnumeration = klass;

  rb_include_module(klass, rb_const_get(rb_cObject, rb_intern("Enumerable")));
  rb_define_method(klass, "each", each, 0);
  rb_define_method(klass, "to_a", to_a, 0);
  rb_define_method(klass, "size", size, 0);
  rb_define_method(klass, "simple_type", simple_type, 0);
  rb_define_method(klass, "client", client, 0);
}
