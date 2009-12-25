
#include "sfcc.h"
#include "cimc_enumeration.h"
#include "cimc_object_path.h"

static void
dealloc(CIMCEnumeration *enm)
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
  CIMCEnumeration *ptr = NULL;
  CIMCData next;
  Data_Get_Struct(self, CIMCEnumeration, ptr);

  while (ptr->ft->hasNext(ptr, NULL)) {
    next = ptr->ft->getNext(ptr, NULL);
    rb_yield(sfcc_cimcdata_to_value(next));
  }
  return Qnil;
}

VALUE
Sfcc_wrap_cimc_enumeration(CIMCEnumeration *enm)
{
  SFCC_INC_REFCOUNT(enm);
  return Data_Wrap_Struct(cSfccCimcEnumeration, NULL, dealloc, enm);
}

VALUE cSfccCimcEnumeration;
void init_cimc_enumeration()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  VALUE klass = rb_define_class_under(cimc, "Enumeration", rb_cObject);
  cSfccCimcEnumeration = klass;

  rb_define_method(klass, "each", each, 0);
  rb_include_module(klass, rb_const_get(rb_cObject, rb_intern("Enumerable"))); 
}
