
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
  CIMCEnumeration *ptr, *tmp;
  CIMCData next;
  Data_Get_Struct(self, CIMCEnumeration, ptr);

  /* clone, since getNext() changes the Enumeration */
  tmp = ptr->ft->clone(ptr, &status);

  if (!status.rc) {
    while (tmp->ft->hasNext(tmp, NULL)) {
      VALUE value;
      next = tmp->ft->getNext(tmp, NULL);
      value = sfcc_cimdata_to_value(next);
      rb_yield(value);
    }
  }

  tmp->ft->release(tmp);
  sfcc_rb_raise_if_error(status, "Can't iterate enumeration");
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
  rb_include_module(klass, rb_const_get(rb_cObject, rb_intern("Enumerable")));
}
