#include "sfcc.h"
#include "cim_args.h"

static void
dealloc(CMPIArgs *args)
{
  /*SFCC_DEC_REFCOUNT(args);*/
}

/**
 * call-seq:
 *   add_arg
 * Adds an argument to the arguments
 */
static VALUE add_arg(VALUE self, VALUE name, VALUE value)
{
  CMPIArgs *ptr = NULL;
  CMPIData data;
  CMPIStatus status;
  Data_Get_Struct(self, CMPIArgs, ptr);
  data = sfcc_value_to_cimdata(value);
  status = ptr->ft->addArg(ptr, StringValuePtr(name), &data.value, data.type); 
  sfcc_rb_raise_if_error(status, "Can't add argument");
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
  CMPIArgs *ptr = NULL;
  CMPIStatus status;
  Data_Get_Struct(self, CMPIArgs, ptr);
  count = ptr->ft->getArgCount(ptr, &status); 
  sfcc_rb_raise_if_error(status, "Can't add argument");
  return UINT2NUM(count);
}

/**
 * call-seq
 *   args.to_hash
 * Creates a hash with the contained arguments
 */
static VALUE to_hash(VALUE self)
{
  CMPIArgs *ptr = NULL;
  int i = 0;
  int n = 0;
  VALUE hash;
  CMPIData argname;
  CMPIData argdata;
  CMPIStatus status;

  Data_Get_Struct(self, CMPIArgs, ptr);
  n = ptr->ft->getArgCount(ptr, NULL);  
  hash = rb_hash_new();
  for (; i < n; ++i) {
    argname.type = CMPI_string;
    argname.state = CMPI_goodValue;
    argdata = ptr->ft->getArgAt(ptr, i, &argname.value.string, &status);
    if (!status.rc) {
      rb_hash_aset(hash, sfcc_cimdata_to_value(argname), sfcc_cimdata_to_value(argdata));
    }
    else {
      sfcc_rb_raise_if_error(status, "Can't retrieve argument");
      return Qnil;
    }
  }
  return hash;
}

/**
 * call-seq
 *   new
 * creates a new arguments container
 */
static VALUE new(VALUE klass)
{
  CMPIStatus status;
  CMPIArgs *ptr = newCMPIArgs(&status);
  if (!status.rc) {
    /*  return Sfcc_wrap_cim_args(ptr->ft->clone(ptr, &status));*/
    return Sfcc_wrap_cim_args(ptr);
  }
  sfcc_rb_raise_if_error(status, "Can't create CIM arguments");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_args(CMPIArgs *args)
{
  SFCC_INC_REFCOUNT(args);
  return Data_Wrap_Struct(cSfccCimArgs, NULL, dealloc, args);
}

VALUE cSfccCimArgs;
void init_cim_args()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cim = rb_define_module_under(sfcc, "Cim");

  /**
   * a string in the CIM environemtn
   */
  VALUE klass = rb_define_class_under(cim, "Args", rb_cObject);
  cSfccCimArgs = klass;

  rb_define_singleton_method(klass, "new", new, 0);
  rb_define_method(klass, "add_arg", add_arg, 2);
  rb_define_method(klass, "arg_count", arg_count, 0);
  rb_define_method(klass, "to_hash", to_hash, 0);
}
