
#include "cim_data.h"

static void
dealloc(CIMCData *data)
{
/*  fprintf(stderr, "Sfcc_dealloc_data %p\n", data); */
  free(data);
}

/**
 * call-seq:
 *   state()
 *
 * Get the state of the data
 */
static VALUE state(VALUE self)
{
  CIMCData *data;
  Data_Get_Struct(self, CIMCData, data);
  return UINT2NUM(data->state);
}

/**
 * call-seq:
 *   type()
 *
 * Get the type of the data
 */
static VALUE type(VALUE self)
{
  CIMCData *data;
  Data_Get_Struct(self, CIMCData, data);
  return UINT2NUM(data->type);
}

/**
 * call-seq:
 *   value()
 *
 * Get the value of the data
 */
static VALUE value(VALUE self)
{
  CIMCData *data;
  Data_Get_Struct(self, CIMCData, data);
  return sfcc_cimdata_to_value(data, NULL);
}


VALUE
Sfcc_wrap_cim_data(CIMCData *data)
{
  CIMCData *ptr = (CIMCData *)malloc(sizeof(CIMCData));
  if (!ptr)
    rb_raise(rb_eNoMemError, "Cannot alloc rb_sfcc_data");
  memcpy(ptr, data, sizeof(CIMCData));
  return Data_Wrap_Struct(cSfccCimData, NULL, dealloc, ptr);
}


VALUE cSfccCimData;
void init_cim_data()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * data on the CIM namespace
   */
  VALUE klass = rb_define_class_under(cimc, "Data", rb_cObject);
  cSfccCimData = klass;

  rb_define_method(klass, "state", state, 0);
  rb_define_method(klass, "type", type, 0);
  rb_define_method(klass, "value", value, 0);
}
