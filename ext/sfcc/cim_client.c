#include "cim_client.h"
#include "cim_enumeration.h"
#include "cim_object_path.h"
#include "cim_class.h"
#include "cim_instance.h"

static void
dealloc(CMCIClient *client)
{
  SFCC_DEC_REFCOUNT(client);
}

/**
 * call-seq:
 *  class_names(object_path, flags)
 *
 * return the available class names for the given
 * +object_path+ and +flags+
 */
static VALUE class_names(VALUE self, VALUE object_path, VALUE flags)
{
  CMPIStatus status;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  VALUE rbenm = Qnil;

  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, CMPIObjectPath, op);

  CMPIEnumeration *enm = client->ft->enumClassNames(client, op, NUM2INT(flags), &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm);
  }
  //enm->ft->release(enm);
  sfcc_rb_raise_if_error(status, "Can't get class names");
  return rbenm;
}

/**
 * call-seq:
 *  classes(object_path, flags)
 *
 * classes and subclasses in the namespace defined by +object_path+.
 * Class structure and inheritance scope can be controled using the +flags+ parameter
 * Any combination of the following flags are supported:
 * Flags::LocalOnly, Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 */
static VALUE classes(VALUE self, VALUE object_path, VALUE flags)
{
  CMPIStatus status;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  VALUE rbenm = Qnil;

  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, CMPIObjectPath, op);

  CMPIEnumeration *enm = client->ft->enumClasses(client, op, NUM2INT(flags), &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm);
  }
  //enm->ft->release(enm);
  sfcc_rb_raise_if_error(status, "Can't get classes");
  return rbenm;
}

/**
 * call-seq:
 */
static VALUE query(VALUE self,
                   VALUE object_path,
                   VALUE query,
                   VALUE lang)
{
  CMPIStatus status;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;

  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, CMPIObjectPath, op);
  VALUE rbenm = Qnil;

  CMPIEnumeration *enm = client->ft->execQuery(client,
                                               op,
                                               StringValuePtr(query),
                                               StringValuePtr(lang),
                                               &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm);
  }
  //enm->ft->release(enm);
  sfcc_rb_raise_if_error(status, "Can't get instances from query");
  return rbenm;
}

/**
 * call-seq:
 *  instance_names(object_path, flags)
 *
 * instance names of the class defined by +object_path+
 */
static VALUE instance_names(VALUE self, VALUE object_path)
{
  CMPIStatus status;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  VALUE rbenm = Qnil;

  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, CMPIObjectPath, op);

  CMPIEnumeration *enm = client->ft->enumInstanceNames(client, op, &status);

  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm);
  }
  sfcc_rb_raise_if_error(status, "Can't get instance names");
  return rbenm;
}

/**
 * call-seq:
 *  instances(object_path, flags=0, properties=nil)
 *
 * Enumerate the instance names of the class defined by +object_path+
 * +object_path+ ObjectPath containing nameSpace and classname components.
 * +flags+ Any combination of the following flags are supported:
 * Flags::LocalOnly, Flags::DeepInheritance,
 * Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 * +properties+ If not NULL, the members of the array define one or more 
 * Property names.
 * Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 * 
 */
static VALUE instances(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE flags;
  VALUE properties;

  CMPIStatus status;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  char **props;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);
  if (NIL_P(flags)) flags = INT2NUM(0);

  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, CMPIObjectPath, op);

  props = sfcc_value_array_to_string_array(properties);

  CMPIEnumeration *enm = client->ft->enumInstances(client, op, NUM2INT(flags), props, &status);

  free(props);

  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm);
  }
  sfcc_rb_raise_if_error(status, "Can't get instances");
  return rbenm;
}

/**
 * call-seq:
 *   invoke_method(object_path, method_name, argin, argout)
 * Invoke a named, extrinsic method of an instance defined by +object_path+
 *
 * +object_path+ containing namespace, classname, and key
 * components.
 * +argin+ hash containing the input parameters (keys can be symbols) or
 * strings.
 * +argout+ hash where output parameters will be returned
 */
static VALUE invoke_method(VALUE self,
                           VALUE object_path,
                           VALUE method_name,
                           VALUE argin,
                           VALUE argout)
{
  CMPIStatus status;
  CMCIClient *ptr = NULL;
  CMPIObjectPath *op = NULL;
  CMPIArgs *cmpiargsout;
  VALUE method_name_str;
  char *method_name_cstr;
  CMPIData ret;
  Check_Type(argin, T_HASH);
  memset(&status, 0, sizeof(CMPIStatus));

  cmpiargsout = newCMPIArgs(NULL);

  Data_Get_Struct(self, CMCIClient, ptr);
  Data_Get_Struct(object_path, CMPIObjectPath, op);

  method_name_str = rb_funcall(method_name, rb_intern("to_s"), 0);
  method_name_cstr = StringValuePtr(method_name_str);
  ret = ptr->ft->invokeMethod(ptr,
                              op,
                              method_name_cstr,
                              sfcc_hash_to_cimargs(argin),
                              cmpiargsout,
                              &status);
  if (!status.rc) {
    if (cmpiargsout && ! NIL_P(argout)) {
      Check_Type(argout, T_HASH);
      rb_funcall(argout, rb_intern("merge!"), 1, sfcc_cimargs_to_hash(cmpiargsout));
    }
    return sfcc_cimdata_to_value(ret);
  }
  sfcc_rb_raise_if_error(status, "Can't invoke method '%s'", method_name_cstr);
  return Qnil;
}

/**
 * call-seq:
 *   set_property(object_path, name, value)
 * Sets the named property value of an instance defined by
 * +object_path+
 *
 * +object_path+ containing namespace, classname, and key
 * components.
 */
static VALUE set_property(VALUE self,
                          VALUE object_path,
                          VALUE name,
                          VALUE value)
{
  CMCIClient *ptr = NULL;
  CMPIObjectPath *op = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMCIClient, ptr);
  Data_Get_Struct(object_path, CMPIObjectPath, op);
  data = sfcc_value_to_cimdata(value);
  ptr->ft->setProperty(ptr, op, StringValuePtr(name), &data.value, data.type);
  return value;
}

/**
 * call-seq:
 *   property(object_path, name)
 *
 * Gets the named property value of an instance defined by
 * +object_path+
 *
 * +object_path+ containing namespace, classname, and key
 * components.
 */
static VALUE property(VALUE self, VALUE object_path, VALUE name)
{
  CMCIClient *ptr = NULL;
  CMPIObjectPath *op = NULL;
  CMPIStatus status;
  CMPIData data;
  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMCIClient, ptr);
  Data_Get_Struct(object_path, CMPIObjectPath, op);
  data = ptr->ft->getProperty(ptr, op, StringValuePtr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", StringValuePtr(name));
  return Qnil;
}

/**
 * call-seq:
 *  get_class(object_path, flags=0, properties=nil)
 *
 * Get Class using +object_path+ as reference. Class structure can be
 * controled using the flags parameter.
 *
 * +object_path+ ObjectPath containing nameSpace and classname components.
 * +flags+ Any combination of the following flags are supported:
 *  Flags::LocalOnly, Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 * +properties+ If not nil, the members of the array define one or more Property
 * names. Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 */
static VALUE get_class(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE flags;
  VALUE properties;

  CMPIStatus status;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  CMPIConstClass *cimclass = NULL;
  char **props;

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);

  if (NIL_P(flags)) flags = INT2NUM(0);

  memset(&status, 0, sizeof(CMPIStatus));
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, CMPIObjectPath, op);

  props = sfcc_value_array_to_string_array(properties);

  cimclass = client->ft->getClass(client, op, NUM2INT(flags), props, &status);

  free(props);

  sfcc_rb_raise_if_error(status, "Can't get class");
  return Sfcc_wrap_cim_class(cimclass);
}

static VALUE connect(VALUE klass, VALUE host, VALUE scheme, VALUE port, VALUE user, VALUE pwd)
{
  CMCIClient *client = NULL;
  CMPIStatus status;
  client = cmciConnect(NIL_P(host) ? NULL : StringValuePtr(host),
                       NIL_P(scheme) ? NULL : StringValuePtr(scheme),
                       NIL_P(port) ? NULL : StringValuePtr(port),
                       NIL_P(user) ? NULL : StringValuePtr(user),
                       NIL_P(pwd) ? NULL : StringValuePtr(pwd),
                       &status);
  if ( !status.rc )
    return Sfcc_wrap_cim_client(client);
  sfcc_rb_raise_if_error(status, "Can't create CIM client");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_client(CMCIClient *client)
{
  assert(client);
  SFCC_INC_REFCOUNT(client);
  return Data_Wrap_Struct(cSfccCimClient, NULL, dealloc, client);
}

VALUE cSfccCimClient;
void init_cim_client()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * CIM client which can communicate with a CIMOM
   */
  VALUE klass = rb_define_class_under(cimc, "Client", rb_cObject);
  cSfccCimClient = klass;

  rb_define_singleton_method(klass, "native_connect", connect, 5);
  rb_define_method(klass, "class_names", class_names, 2);
  rb_define_method(klass, "classes", classes, 2);
  rb_define_method(klass, "query", query, 3);
  rb_define_method(klass, "instance_names", instance_names, 1);
  rb_define_method(klass, "instances", instances, -1);
  rb_define_method(klass, "get_class", get_class, -1);
  rb_define_method(klass, "invoke_method", invoke_method, 4);
  rb_define_method(klass, "set_property", set_property, 3);
  rb_define_method(klass, "property", property, 2);
}
