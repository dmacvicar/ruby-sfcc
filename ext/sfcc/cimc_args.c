
#include "cimc_args.h"

static void
dealloc(CIMCArgs *args)
{
  /*SFCC_DEC_REFCOUNT((CMPIArgs *) args);*/
  CMRelease((CMPIArgs *)args);
}

/**
 * call-seq:
 *   add_arg
 * Adds an argument to the arguments
 */
static VALUE add_arg(VALUE self, VALUE name, VALUE value)
{
  CIMCArgs *ptr = NULL;
  CIMCData data;
  CMPIStatus status;
  Data_Get_Struct(self, CIMCArgs, ptr);
  data = sfcc_value_to_cimcdata(value);
  status = ((CMPIArgs *)ptr)->ft->addArg((CMPIArgs *)ptr, StringValuePtr(name), (CMPIValue *) &data.value, data.type); 
  /*sfcc_rb_raise_if_error((CIMCStatus)status, "Can't add argument");*/
  return self;
}

/**
 * call-seq:
 *   arg_count
 * number of arguments
 */
static VALUE arg_count(VALUE self)
{
  unsigned int count = 0;
  CIMCArgs *ptr = NULL;
  CIMCStatus status;
  Data_Get_Struct(self, CIMCArgs, ptr);
  count = ((CMPIArgs *)ptr)->ft->getArgCount((CMPIArgs *)ptr, (CMPIStatus *)&status); 
  sfcc_rb_raise_if_error(status, "Can't add argument");
  return UINT2NUM(count);
}

/**
 * call-seq:
 *   args.each_arg do |arg|
 *     ...
 *   end
 * enumerates all arguments
 */
static VALUE each_arg(VALUE self)
{
  unsigned int count = 0;
  CIMCArgs *ptr = NULL;
  CIMCStatus status;
  int i = 0;
  /*CIMCData data;*/
  Data_Get_Struct(self, CIMCArgs, ptr);

  count = ((CMPIArgs *)ptr)->ft->getArgCount((CMPIArgs *)ptr, (CMPIStatus *)&status);
  for (; i < count; ++i) {
    /*data = sfcc_cimcdata_to_value( CMGetArgAt((CMPIArgs *)ptr, i
      rb_yield(sfcc_cimcdata_to_value(((CMPIArgs *)ptr)->ft->getArgAt((CMPIArgs *)ptr, (CMPIStatus *) &status);))*/
  }
  sfcc_rb_raise_if_error(status, "Can't add argument");
  return Qnil;
}

VALUE
Sfcc_wrap_cimc_args(CIMCArgs *args)
{
  SFCC_INC_REFCOUNT(args);
  return Data_Wrap_Struct(cSfccCimcArgs, NULL, dealloc, args);
}

VALUE cSfccCimcArgs;
void init_cimc_args()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  /**
   * a string in the CIM environemtn
   */
  VALUE klass = rb_define_class_under(cimc, "Args", rb_cObject);
  cSfccCimcArgs = klass;

  rb_define_method(klass, "add_arg", add_arg, 2);
  rb_define_method(klass, "arg_count", arg_count, 0);
}
