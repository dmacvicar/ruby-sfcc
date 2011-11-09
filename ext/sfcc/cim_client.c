#include "cim_client.h"
#include "cim_enumeration.h"
#include "cim_object_path.h"
#include "cim_class.h"
#include "cim_instance.h"

static void
dealloc(CMCIClient *client)
{
  fprintf(stderr, "Sfcc_dealloc_cim_client %p\n", client);
  SFCC_DEC_REFCOUNT(client);
}

/**
 * call-seq:
 *  get_class(object_path, flags=0, properties=nil)
 *
 * Get Class using +object_path+ as reference. Class structure can be
 * controled using the flags parameter.
 *
 * +object_path+ ObjectPath containing nameSpace and classname components.
 *
 * +flags+ Any combination of the following flags are supported:
 *  Flags::LocalOnly, Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 *
 * +properties+ If not nil, the members of the array define one or more Property
 * names. Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 */
static VALUE get_class(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE flags;
  VALUE properties;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  CMPIConstClass *cimclass = NULL;
  CMPIConstClass *cimclassnew = NULL;
  char **props;

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);

  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  props = sfcc_value_array_to_string_array(properties);
  cimclass = client->ft->getClass(client, op, NUM2INT(flags), props, &status);
  free(props);

  if (!status.rc) {
      cimclassnew = cimclass->ft->clone(cimclass, NULL);
      cimclass->ft->release(cimclass);
      return Sfcc_wrap_cim_class(cimclassnew, self);
  }
  sfcc_rb_raise_if_error(status, "Can't get class at %s", CMGetCharsPtr(CMObjectPathToString(op, NULL), NULL));
  return Qnil;
}

/**
 * call-seq:
 *  class_names(object_path, flags=0)
 *
 * return the available class names for the given
 * +object_path+ and +flags+
 *
 * The following flag is supported: Flags::DeepInheritance.
 */
static VALUE class_names(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE flags;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "11", &object_path, &flags);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  CMPIEnumeration *enm = client->ft->enumClassNames(client, op, NUM2INT(flags), &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }

  sfcc_rb_raise_if_error(status, "Can't get class names");
  return Qnil;
}

/**
 * call-seq:
 *  classes(object_path, flags=0)
 *
 * classes and subclasses in the namespace defined by +object_path+.
 *
 * Class structure and inheritance scope can be controled using the +flags+ parameter
 * Any combination of the following flags are supported:
 * Flags::LocalOnly, Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 */
static VALUE classes(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE flags;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "11", &object_path, &flags);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  CMPIEnumeration *enm = client->ft->enumClasses(client, op, NUM2INT(flags), &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }

  sfcc_rb_raise_if_error(status, "Can't get classes, try increasing maxMsgLen in sfcb.cfg ?");
  return Qnil;
}

/**
 * call-seq:
 *  get_instance(object_path, flags=0, properties=nil)
 *
 * get instance using +object_path+ as reference. Instance structure
 * can be controlled using the flags parameter.
 *
 * +object_path+ an ObjectPath containing namespace, class name and key
 * components.
 *
 * +flags+ Any combination of the following flags are supported:
 * Flags::LocalOnly, Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 *
 * +properties+ If not nil, the members of the array define one or more Property
 * names. Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 */
static VALUE get_instance(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE flags;
  VALUE properties;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  CMPIInstance *ciminstance = NULL;
  char **props;

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  props = sfcc_value_array_to_string_array(properties);

  ciminstance = client->ft->getInstance(client, op, NUM2INT(flags), props, &status);
  free(props);

  if (!status.rc)
    return Sfcc_wrap_cim_instance(ciminstance->ft->clone(ciminstance, NULL), self);

  sfcc_rb_raise_if_error(status, "Can't get instance");
  return Qnil;
}

/**
 * call-seq:
 *   create_instance(object_path, instance)
 *
 * Create Instance from +object_path+ as reference.
 *
 * +object_path+ ObjectPath containing nameSpace, classname and key components.
 *
 * +instance+ Complete instance.
 *
 * returns the assigned instance reference (object path)
 */
static VALUE create_instance(VALUE self, VALUE object_path, VALUE instance)
{
  CMPIStatus status = {CMPI_RC_OK, NULL};
  CMCIClient *ptr = NULL;
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMPIObjectPath *new_op = NULL;
  CMPIInstance *inst = NULL;

  Data_Get_Struct(self, CMCIClient, ptr);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;
  Data_Get_Struct(instance, struct mark_struct, obj);
  instance = (CMPIInstance *)obj->cmpi_object;

  new_op = ptr->ft->createInstance(ptr, op, inst, &status);

  if (!status.rc)
    return Sfcc_wrap_cim_object_path(new_op->ft->clone(new_op, NULL), self);

  sfcc_rb_raise_if_error(status, "Can't create instance");
  return Qnil;
}

/**
 * call-seq:
 *  set_instance(object_path, instance, flags=0, properties=nil)
 *
 * Replace an existing Instance from +instance+, using +object_path+ as reference.
 *
 * +object_path+ ObjectPath containing nameSpace, classname and key components.
 *
 * +instance+ Complete instance.
 *
 * +flags+ The following flag is supported: Flags::IncludeQualifiers.
 *
 * + properties+ If not nil, the members of the array define one or more Property
 * names, only those properties will be updated. Else, all properties will be updated.
 */
static VALUE set_instance(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE instance;
  VALUE flags;
  VALUE properties;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMPIInstance *inst = NULL;
  CMCIClient *client = NULL;
  char **props;

  rb_scan_args(argc, argv, "22", &object_path, &instance, &flags, &properties);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;
  Data_Get_Struct(instance, struct mark_struct, obj);
  instance = (CMPIInstance *)obj->cmpi_object;

  props = sfcc_value_array_to_string_array(properties);

  status = client->ft->setInstance(client, op, inst, NUM2INT(flags), props);
  free(props);

  sfcc_rb_raise_if_error(status, "Can't set instance");
  return instance;
}

/**
 * call-seq:
 *  delete_instance(object_path)
 *
 * Delete an existing Instance using +object_path+ as reference.
 * +object_path+ ObjectPath containing nameSpace, classname and key components.
 */
static VALUE delete_instance(VALUE self, VALUE object_path)
{
  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  status = client->ft->deleteInstance(client, op);
  sfcc_rb_raise_if_error(status, "Can't delete instance '%s'", CMGetCharsPtr(CMObjectPathToString(op, NULL), NULL));
  return Qnil;
}

/**
 * call-seq:
 *   query(object_path, query, lang)
 *
 * Query the enumeration of instances of the class (and subclasses) defined
 * by +object_path+ using +query+ expression.
 *
 * +object_path+ ObjectPath containing nameSpace and classname components.
 *
 * +query+ Query expression
 *
 * +lang+ Query Language
 *
 * returns resulting eumeration of instances
 */
static VALUE query(VALUE self,
                   VALUE object_path,
                   VALUE query,
                   VALUE lang)
{
  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;
  VALUE rbenm = Qnil;

  CMPIEnumeration *enm = client->ft->execQuery(client,
                                               op,
                                               to_charptr(query),
                                               to_charptr(lang),
                                               &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }

  sfcc_rb_raise_if_error(status, "Can't get instances from query");
  return Qnil;
}

/**
 * call-seq:
 *  instance_names(object_path, flags)
 *
 * instance names of the class defined by +object_path+
 */
static VALUE instance_names(VALUE self, VALUE object_path)
{
  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op;
  CMCIClient *client;
  VALUE rbenm = Qnil;

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  CMPIEnumeration *enm = client->ft->enumInstanceNames(client, op, &status);

  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }
  sfcc_rb_raise_if_error(status, "Can't get instance names");
  return Qnil;
}

/**
 * call-seq:
 *  instances(object_path, flags=0, properties=nil)
 *
 * Enumerate the instance names of the class defined by +object_path+
 *
 * +object_path+ ObjectPath containing nameSpace and classname components.
 *
 * +flags+ Any combination of the following flags are supported:
 * Flags::LocalOnly, Flags::DeepInheritance,
 * Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 *
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

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  char **props;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  props = sfcc_value_array_to_string_array(properties);

  CMPIEnumeration *enm = client->ft->enumInstances(client, op, NUM2INT(flags), props, &status);

  free(props);

  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }

  sfcc_rb_raise_if_error(status, "Can't get instances");
  return Qnil;
}

/**
 * call-seq:
 *  associators(object_path, association_class=nil,
 *              result_class=nil, role=nil, result_role=nil, flags=0
 *              properties=nil)
 *
 * Enumerate instances associated with the Instance defined by the +object_path+
 * 
 * +object_path+ Source ObjectPath containing nameSpace, classname
 * and key components.
 *
 * +association_class+ If not nil, MUST be a valid Association Class name.
 * It acts as a filter on the returned set of Objects by mandating that
 * each returned Object MUST be associated to the source Object via an
 * Instance of this Class or one of its subclasses.
 *
 * +result_class+ If not nil, MUST be a valid Class name.
 * It acts as a filter on the returned set of Objects by mandating that
 * each returned Object MUST be either an Instance of this Class (or one
 * of its subclasses).
 *
 * +role+ If not nil, MUST be a valid Property name.
 * It acts as a filter on the returned set of Objects by mandating
 * that each returned Object MUST be associated to the source Object
 * via an Association in which the source Object plays the specified role
 * (i.e. the name of the Property in the Association Class that refers
 * to the source Object MUST match the value of this parameter).
 *
 * +result_role+ If not nil, MUST be a valid Property name.
 * It acts as a filter on the returned set of Objects by mandating
 * that each returned Object MUST be associated to the source Object
 * via an Association in which the returned Object plays the specified role
 * (i.e. the name of the Property in the Association Class that refers to
 * the returned Object MUST match the value of this parameter).
 *
 * +flags+ Any combination of the following flags are supported:
 * Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 *
 * +properties+ If not nil, the members of the array define one or more Property
 * names. Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 *
 * returns enumeration of instances
 */
static VALUE associators(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE assoc_class;
  VALUE result_class;
  VALUE role;
  VALUE result_role;
  VALUE flags;
  VALUE properties;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  char **props;
  CMPIEnumeration *enm = NULL;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "16", &object_path,
               &assoc_class, &result_class,
               &role, &result_role, &flags, &properties);

  if (NIL_P(flags)) flags = INT2NUM(0);
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  props = sfcc_value_array_to_string_array(properties);

  enm = client->ft->associators(client,
                                op,
                                to_charptr(assoc_class),
                                to_charptr(result_class),
                                to_charptr(role),
                                to_charptr(result_role),
                                NUM2INT(flags), props, &status);
  free(props);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }

  sfcc_rb_raise_if_error(status, "Can't get associators for '%s'", CMGetCharsPtr(CMObjectPathToString(op, NULL), NULL));
  return Qnil;
}

/**
 * call-seq:
 *  associator_names(object_path, association_class=nil,
 *                   result_class=nil, role=nil, result_role=nil, flags=0
 *                   properties=nil)
 *
 * Enumerate object paths associated with the Instance defined by the
 * +object_path+
 * 
 * +object_path+ Source ObjectPath containing nameSpace, classname
 * and key components.
 *
 * +association_class+ If not nil, MUST be a valid Association Class name.
 * It acts as a filter on the returned set of Objects by mandating that
 * each returned Object MUST be associated to the source Object via an
 * Instance of this Class or one of its subclasses.
 *
 * +result_class+ If not nil, MUST be a valid Class name.
 * It acts as a filter on the returned set of Objects by mandating that
 * each returned Object MUST be either an Instance of this Class (or one
 * of its subclasses).
 *
 * +role+ If not nil, MUST be a valid Property name.
 * It acts as a filter on the returned set of Objects by mandating
 * that each returned Object MUST be associated to the source Object
 * via an Association in which the source Object plays the specified role
 * (i.e. the name of the Property in the Association Class that refers
 * to the source Object MUST match the value of this parameter).
 *
 * +result_role+ If not nil, MUST be a valid Property name.
 * It acts as a filter on the returned set of Objects by mandating
 * that each returned Object MUST be associated to the source Object
 * via an Association in which the returned Object plays the specified role
 * (i.e. the name of the Property in the Association Class that refers to
 * the returned Object MUST match the value of this parameter).
 *
 * returns enumeration of object paths
 */
static VALUE associator_names(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE assoc_class;
  VALUE result_class;
  VALUE role;
  VALUE result_role;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  CMPIEnumeration *enm = NULL;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "14", &object_path,
               &assoc_class, &result_class,
               &role, &result_role);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  enm = client->ft->associatorNames(client,
                                    op,
                                    to_charptr(assoc_class),
                                    to_charptr(result_class),
                                    to_charptr(role),
                                    to_charptr(result_role),
                                    &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }
  sfcc_rb_raise_if_error(status, "Can't get associator names for '%s'", CMGetCharsPtr(CMObjectPathToString(op, NULL), NULL));
  return Qnil;
}

/**
 * call-seq:
 *  references(object_path, result_class=nil, role=nil, flags=0, properties=nil)
 *
 * Enumerates the association instances that refer to the instance defined by
 * +object_path+
 *
 * +object_path+ Source ObjectPath containing nameSpace, classname and key components.
 *
 * +result_class+ If not nil, MUST be a valid Class name.
 * It acts as a filter on the returned set of Objects by mandating that
 * each returned Object MUST be either an Instance of this Class (or one
 * of its subclasses).
 *
 * +role+ If not nil, MUST be a valid Property name.
 * It acts as a filter on the returned set of Objects by mandating
 * that each returned Object MUST be associated to the source Object
 * via an Association in which the source Object plays the specified role
 * (i.e. the name of the Property in the Association Class that refers
 * to the source Object MUST match the value of this parameter).
 *
 * +flags+ Any combination of the following flags are supported:
 * Flags::IncludeQualifiers and Flags::IncludeClassOrigin.
 *
 * +properties+ If not nil, the members of the array define one or more Property
 * names. Each returned Object MUST NOT include elements for any Properties
 * missing from this list
 *
 * returns enumeration of ObjectPaths
 */
static VALUE references(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path;
  VALUE result_class;
  VALUE role;
  VALUE flags;
  VALUE properties;

  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  char **props;
  CMPIEnumeration *enm = NULL;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "14", &object_path,
               &result_class, &role,
               &flags, &properties);

  if (NIL_P(flags)) flags = INT2NUM(0);
  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  props = sfcc_value_array_to_string_array(properties);

  enm = client->ft->references(client,
                               op,
                               to_charptr(result_class),
                               to_charptr(role),
                               NUM2INT(flags), props, &status);
  free(props);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, NULL), self);
    enm->ft->release(enm);
    return rbenm;
  }
  sfcc_rb_raise_if_error(status, "Can't get references for '%s'", CMGetCharsPtr(CMObjectPathToString(op, NULL), NULL));
  return Qnil;
}

/**
 * call-seq:
 *  reference_names(object_path, result_class=nil, role=nil, flags=0, properties=nil)
 *
 * Enumerates the association instances that refer to the instance defined by
 * +object_path+
 *
 * +object_path+ Source ObjectPath containing nameSpace, classname and key components.
 *
 * +result_class+ If not nil, MUST be a valid Class name.
 * It acts as a filter on the returned set of Objects by mandating that
 * each returned Object MUST be either an Instance of this Class (or one
 * of its subclasses).
 *
 * +role+ If not nil, MUST be a valid Property name.
 * It acts as a filter on the returned set of Objects by mandating
 * that each returned Object MUST be associated to the source Object
 * via an Association in which the source Object plays the specified role
 * (i.e. the name of the Property in the Association Class that refers
 * to the source Object MUST match the value of this parameter).
 *
 * returns enumeration of ObjectPaths
 */
static VALUE reference_names(int argc, VALUE *argv, VALUE self)
{
  VALUE object_path = Qnil;
  VALUE result_class = Qnil;
  VALUE role = Qnil;
  
  CMPIStatus status = {CMPI_RC_OK, NULL};
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMCIClient *client = NULL;
  CMPIEnumeration *enm = NULL;
  VALUE rbenm = Qnil;

  rb_scan_args(argc, argv, "12", &object_path,
               &result_class, &role);

  Data_Get_Struct(self, CMCIClient, client);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  enm = client->ft->referenceNames(client,
                                   op,
                                   to_charptr(result_class),
                                   to_charptr(role),
                                   &status);
  if (enm && !status.rc ) {
    rbenm = Sfcc_wrap_cim_enumeration(enm->ft->clone(enm, &status), self);
    enm->ft->release(enm);
    return rbenm;
  }
  sfcc_rb_raise_if_error(status, "Can't get reference names for '%s'", CMGetCharsPtr(CMObjectPathToString(op, NULL), NULL));
  return Qnil;
}

/**
 * call-seq:
 *   invoke_method(object_path, method_name, argin, argout)
 *
 * Invoke a named, extrinsic method of an instance defined by +object_path+
 *
 * +object_path+ containing namespace, classname, and key
 * components.
 *
 * +argin+ hash containing the input parameters (keys can be symbols) or
 * strings.
 *
 * +argout+ hash where output parameters will be returned
 */
static VALUE invoke_method(VALUE self,
                           VALUE object_path,
                           VALUE method_name,
                           VALUE argin,
                           VALUE argout)
{
  CMPIStatus status = {CMPI_RC_OK, NULL};
  CMCIClient *ptr = NULL;
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMPIArgs *cmpiargsout;
  VALUE method_name_str;
  const char *method_name_cstr;
  CMPIData ret;
  Check_Type(argin, T_HASH);

  cmpiargsout = newCMPIArgs(NULL);

  Data_Get_Struct(self, CMCIClient, ptr);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;

  method_name_str = rb_funcall(method_name, rb_intern("to_s"), 0);
  method_name_cstr = to_charptr(method_name_str);
  ret = ptr->ft->invokeMethod(ptr,
                              op,
                              method_name_cstr,
                              sfcc_hash_to_cimargs(argin),
                              cmpiargsout,
                              &status);
  if (!status.rc) {
    if (cmpiargsout && ! NIL_P(argout)) {
      Check_Type(argout, T_HASH);
      rb_funcall(argout, rb_intern("merge!"), 1, sfcc_cimargs_to_hash(cmpiargsout, self));
    }
    return sfcc_cimdata_to_value(ret, self);
  }
  sfcc_rb_raise_if_error(status, "Can't invoke method '%s'", method_name_cstr);
  return Qnil;
}

/**
 * call-seq:
 *   set_property(object_path, name, value)
 *
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
  CMPIStatus status = {CMPI_RC_OK, NULL};
  CMCIClient *ptr = NULL;
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMPIData data;
  Data_Get_Struct(self, CMCIClient, ptr);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;
  data = sfcc_value_to_cimdata(value);
  status = ptr->ft->setProperty(ptr, op, to_charptr(name), &data.value, data.type);

  if ( !status.rc )
    return value;
  sfcc_rb_raise_if_error(status, "Can't set property '%s'", to_charptr(name));
  return Qnil;
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
  struct mark_struct *obj;
  CMPIObjectPath *op = NULL;
  CMPIStatus status = {CMPI_RC_OK, NULL};
  CMPIData data;

  Data_Get_Struct(self, CMCIClient, ptr);
  Data_Get_Struct(object_path, struct mark_struct, obj);
  op = (CMPIObjectPath *)obj->cmpi_object;
  data = ptr->ft->getProperty(ptr, op, to_charptr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data, self);

  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", to_charptr(name));
  return Qnil;
}

static VALUE connect(VALUE klass, VALUE host, VALUE scheme, VALUE port, VALUE user, VALUE pwd)
{
  CMCIClient *client = NULL;
  CMPIStatus status = {CMPI_RC_OK, NULL};
  client = cmciConnect(to_charptr(host),
                       to_charptr(scheme),
                       to_charptr(port),
                       to_charptr(user),
                       to_charptr(pwd),
                       &status);
  if ( !status.rc )
    return Sfcc_wrap_cim_client(client);
  sfcc_rb_raise_if_error(status, "Can't create CIM client");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_client(CMCIClient *client)
{
  VALUE ret;
  assert(client);
  SFCC_INC_REFCOUNT(client);
  ret = Data_Wrap_Struct(cSfccCimClient, NULL, dealloc, client);
  fprintf(stderr, "Sfcc_wrap_cim_client %p => %p\n", client, (void *)ret);
  return ret;
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
  rb_define_method(klass, "get_class", get_class, -1);
  rb_define_method(klass, "class_names", class_names, -1);
  rb_define_method(klass, "classes", classes, -1);
  rb_define_method(klass, "get_instance", get_instance, -1);
  rb_define_method(klass, "create_instance", create_instance, 2);
  rb_define_method(klass, "set_instance", set_instance, -1);
  rb_define_method(klass, "delete_instance", delete_instance, 1);  
  rb_define_method(klass, "query", query, 3);
  rb_define_method(klass, "instance_names", instance_names, 1);
  rb_define_method(klass, "instances", instances, -1);
  rb_define_method(klass, "associators", associators, -1);
  rb_define_method(klass, "associator_names", associator_names, -1);
  rb_define_method(klass, "references", references, -1);
  rb_define_method(klass, "reference_names", reference_names, -1);
  rb_define_method(klass, "invoke_method", invoke_method, 4);
  rb_define_method(klass, "set_property", set_property, 3);
  rb_define_method(klass, "property", property, 2);
}
