
#include "sfcc.h"
#include "cim_enumeration.h"
#include "cim_object_path.h"

static void
dealloc(CMPIEnumeration *enm)
{
  SFCC_DEC_REFCOUNT(enm);
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
  CMPIEnumeration *ptr = NULL;
  CMPIData next;
  Data_Get_Struct(self, CMPIEnumeration, ptr);

  while (ptr->ft->hasNext(ptr, NULL)) {
    next = ptr->ft->getNext(ptr, NULL);
    rb_yield(sfcc_cimdata_to_value(next));
  }
  return Qnil;
}

VALUE
Sfcc_wrap_cim_enumeration(CMPIEnumeration *enm)
{
  SFCC_INC_REFCOUNT(enm);
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
  rb_include_module(klass, rb_const_get(rb_cObject, rb_intern("Enumerable"))); 
}
