#include "cimc_client.h"
#include "cimc_enumeration.h"
#include "cimc_object_path.h"
#include "cimc_class.h"
#include "cimc_instance.h"

static void
dealloc(CIMCClient *client)
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
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  VALUE rbenm = Qnil;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  CIMCEnumeration *enm = client->ft->enumClassNames(client, op, NUM2INT(flags), &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cimc_enumeration(enm);
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
 * CIMC_FLAG_LocalOnly, CIMC_FLAG_IncludeQualifiers and CIMC_FLAG_IncludeClassOrigin.
 */
static VALUE classes(VALUE self, VALUE object_path, VALUE flags)
{
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  VALUE rbenm = Qnil;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  CIMCEnumeration *enm = client->ft->enumClasses(client, op, NUM2INT(flags), &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cimc_enumeration(enm);
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
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);
  VALUE rbenm = Qnil;

  CIMCEnumeration *enm = client->ft->execQuery(client,
                                               op,
                                               StringValuePtr(query),
                                               StringValuePtr(lang),
                                               &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cimc_enumeration(enm);
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
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  VALUE rbenm = Qnil;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  CIMCEnumeration *enm = client->ft->enumInstanceNames(client, op, &status);

  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cimc_enumeration(enm);
  }
  //enm->ft->release(enm);
  sfcc_rb_raise_if_error(status, "Can't get instance names");
  return rbenm;
}

/**
 * call-seq:
 *  instances(object_path, flags, properties)
 *
 * Enumerate the instance names of the class defined by +object_path+
 * +object_path+ ObjectPath containing nameSpace and classname components.
 * +flags+ Any combination of the following flags are supported:
 * CIMC_FLAG_LocalOnly, CIMC_FLAG_DeepInheritance,
 * CIMC_FLAG_IncludeQualifiers and CIMC_FLAG_IncludeClassOrigin.
 * +properties+ If not NULL, the members of the array define one or more 
 * Property names.
 * Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 * 
 */
static VALUE instances(VALUE self, VALUE object_path, VALUE flags, VALUE properties)
{
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  char **props;
  VALUE rbenm = Qnil;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  props = sfcc_value_array_to_string_array(properties);

  CIMCEnumeration *enm = client->ft->enumInstances(client, op, NUM2INT(flags), props, &status);

  free(props);

  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cimc_enumeration(enm);
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
  CIMCClient *ptr = NULL;
  CIMCObjectPath *op = NULL;
  CIMCData data;
  /*
  Data_Get_Struct(self, CIMClient_t, ptr);
  Data_Get_Struct(object_path, CIMObjectPath_t, op);
  data = sfcc_value_to_cimcdata(value);
  ptr->ft->setProperty(ptr, op, StringValuePtr(name), &data.value, data.type);
  return value;
  */
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
  CIMCClient *ptr = NULL;
  CIMCObjectPath *op = NULL;
  CIMCData data;
  Data_Get_Struct(self, CIMCClient, ptr);
  Data_Get_Struct(object_path, CIMCObjectPath, op);
  data = sfcc_value_to_cimcdata(value);
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
  CIMCClient *ptr = NULL;
  CIMCObjectPath *op = NULL;
  CIMCStatus status;
  CIMCData data;
  Data_Get_Struct(self, CIMCClient, ptr);
  Data_Get_Struct(object_path, CIMCObjectPath, op);
  data = ptr->ft->getProperty(ptr, op, StringValuePtr(name), &status);
  if ( !status.rc )
    return sfcc_cimcdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", StringValuePtr(name));
  return Qnil;
}

/**
 * call-seq:
 *  get_class(object_path, flags, properties)
 *
 * Get Class using +object_path+ as reference. Class structure can be
 * controled using the flags parameter.
 *
 * +object_path+ ObjectPath containing nameSpace and classname components.
 * +flags+ Any combination of the following flags are supported:
 *  CIMC_FLAG_LocalOnly, CIMC_FLAG_IncludeQualifiers and CIMC_FLAG_IncludeClassOrigin.
 * +properties+ If not nil, the members of the array define one or more Property
 * names. Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 */
static VALUE get_class(VALUE self, VALUE object_path, VALUE flags, VALUE properties)
{
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  CIMCClass *cimclass = NULL;
  char **props;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  props = sfcc_value_array_to_string_array(properties);

  cimclass = client->ft->getClass(client, op, NUM2INT(flags), props, &status);

  free(props);

  sfcc_rb_raise_if_error(status, "Can't get class");
  return Sfcc_wrap_cimc_class(cimclass);
}

VALUE
Sfcc_wrap_cimc_client(CIMCClient *client)
{
  assert(client);
  SFCC_INC_REFCOUNT(client);
  return Data_Wrap_Struct(cSfccCimcClient, NULL, dealloc, client);
}

VALUE cSfccCimcClient;
void init_cimc_client()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  /**
   * CIM client which can communicate with a CIMOM
   */
  VALUE klass = rb_define_class_under(cimc, "Client", rb_cObject);
  cSfccCimcClient = klass;

  rb_define_method(klass, "class_names", class_names, 2);
  rb_define_method(klass, "classes", classes, 2);
  rb_define_method(klass, "query", query, 3);
  rb_define_method(klass, "instance_names", instance_names, 1);
  rb_define_method(klass, "instances", instances, 3);
  rb_define_method(klass, "get_class", get_class, 3);
}

