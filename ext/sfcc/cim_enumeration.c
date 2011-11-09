
#include "sfcc.h"
#include "cim_enumeration.h"
#include "cim_object_path.h"

static void
mark(struct mark_struct *ms)
{
  fprintf(stderr, "Sfcc_mark_cim_enumeration %p, enum %p, client %p\n", ms, ms->cmpi_object, (void *)ms->ruby_value);
  rb_gc_mark(ms->ruby_value);
}

static void
dealloc(struct mark_struct *ms)
{
  fprintf(stderr, "Sfcc_dealloc_cim_enumeration %p, enum %p, client %p\n", ms, ms->cmpi_object, (void *)ms->ruby_value);
  SFCC_DEC_REFCOUNT(((CMPIEnumeration *)ms->cmpi_object));
  free(ms);
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
  CMPIStatus status;
  struct mark_struct *obj;
  CMPIEnumeration *ptr;
  CMPIData next;
  Data_Get_Struct(self, struct mark_struct, obj);
  ptr = (CMPIEnumeration *)obj->cmpi_object;

  CMPIEnumeration *tmp = ptr->ft->clone(ptr, &status);
  
  if (!status.rc) {
    while (tmp->ft->hasNext(tmp, NULL)) {
      next = tmp->ft->getNext(tmp, NULL);
      VALUE cimclass = sfcc_cimdata_to_value(next, obj->ruby_value);
      /* Strange sfcc bug, if I clone the enum, I get a NULL
         class afterwards in the copy */
      if (NIL_P(cimclass))
        continue;
      rb_yield(cimclass);
    }
  }

  tmp->ft->release(tmp);
  sfcc_rb_raise_if_error(status, "Can't iterate enumeration");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_enumeration(CMPIEnumeration *enm, VALUE client)
{
  struct mark_struct *obj = (struct mark_struct *)malloc(sizeof (struct mark_struct));
  obj->cmpi_object = enm;
  obj->ruby_value = client;
  mark(obj);
/*  fprintf(stderr, "Sfcc_wrap_cim_enumeration %p, enum %p, client %p\n", obj, enm, (void *)client); */
  SFCC_INC_REFCOUNT(enm);
  return Data_Wrap_Struct(cSfccCimEnumeration, mark, dealloc, obj);
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
