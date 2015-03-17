#include "cim_client.h"
#include "cim_enumeration.h"
#include "cim_object_path.h"
#include "cim_class.h"
#include "cim_instance.h"

#if HAVE_RUBY_VERSION_H
#include <ruby/version.h>
#endif

#ifdef RUBY_VM
#ifdef HAVE_NATIVETHREAD
#define THREAD_MIGHT_BLOCK 1
#if RUBY_API_VERSION_MAJOR > 1 && RUBY_API_VERSION_MINOR > 0 /* New threading model */
# define THREAD_CALL(function) rb_thread_call_without_gvl((void * (*)(void *))(function), &args, RUBY_UBF_IO, 0);
#else
# define THREAD_CALL(function) rb_thread_blocking_region((rb_blocking_function_t*)(function), &args, RUBY_UBF_IO, 0);
#endif
#else
#define THREAD_MIGHT_BLOCK 0
#endif
#endif

#if THREAD_MIGHT_BLOCK
/*
 * Adapt to Ruby 1.9 thread model
 * 
 * Ruby 1.9 introduces 'real' threads which need special consideration if
 * a called C function is long running (i.e. doing I/O)
 *
 * See http://www.spacevatican.org/2012/7/5/whos-afraid-of-the-big-bad-lock/
 * and esp. http://media.pragprog.com/titles/ruby3/ext_ruby.pdf
 */

/* rb_blocking_function_t only accepts a single argument */
typedef struct {
  CIMCStatus *status;
  CIMCClient *client;
  CIMCObjectPath *op;
  CIMCClass *cimclass;
  CIMCString *ops;
  char **props;
  const char *assoc_class;
  const char *result_class;
  const char *role;
  const char *result_role;
  CIMCFlags flags;
  CIMCInstance *instance;
  const char *query;
  const char *lang;
  const char *name;
  CIMCArgs *argsin;
  CIMCArgs *argsout;
  CIMCData *data;
  CIMCValue *value;
  CIMCType type;
} ruby_thread_args_t;

static CIMCClass *
threaded_get_class(ruby_thread_args_t *args)
{
  return args->client->ft->getClass(args->client, args->op, args->flags, args->props, args->status);
}

static CIMCEnumeration *
threaded_enum_class_names(ruby_thread_args_t *args)
{
  return args->client->ft->enumClassNames(args->client, args->op, args->flags, args->status);
}

static CIMCEnumeration *
threaded_enum_classes(ruby_thread_args_t *args)
{
  return args->client->ft->enumClasses(args->client, args->op, args->flags, args->status);
}

static CIMCInstance *
threaded_get_instance(ruby_thread_args_t *args)
{
  return args->client->ft->getInstance(args->client, args->op, args->flags, args->props, args->status);
}

static CIMCObjectPath *
threaded_create_instance(ruby_thread_args_t *args)
{
  return args->client->ft->createInstance(args->client, args->op, args->instance, args->status);
}

static void *
threaded_set_instance(ruby_thread_args_t *args)
{
  CIMCStatus status;
  status = args->client->ft->setInstance(args->client, args->op, args->instance, args->flags, args->props);
  memcpy(args->status, &status, sizeof(CIMCStatus));
  return NULL;
}

static void *
threaded_delete_instance(ruby_thread_args_t *args)
{
  CIMCStatus status;
  status = args->client->ft->deleteInstance(args->client, args->op);
  memcpy(args->status, &status, sizeof(CIMCStatus));
  return NULL;
}

static CIMCEnumeration *
threaded_exec_query(ruby_thread_args_t *args)
{
  return args->client->ft->execQuery(args->client,
                              args->op,
                              args->query,
                              args->lang,
                              args->status);
}

static CIMCEnumeration *
threaded_enum_instance_names(ruby_thread_args_t *args)
{
  return args->client->ft->enumInstanceNames(args->client, args->op, args->status);
}

static CIMCEnumeration *
threaded_enum_instances(ruby_thread_args_t *args)
{
  return args->client->ft->enumInstances(args->client, args->op, args->flags, args->props, args->status);
}

static CIMCEnumeration *
threaded_associators(ruby_thread_args_t *args)
{
  return args->client->ft->associators(args->client,
                                args->op,
                                args->assoc_class,
                                args->result_class,
                                args->role,
                                args->result_role,
                                args->flags, args->props, args->status);
}

static CIMCEnumeration *
threaded_associator_names(ruby_thread_args_t *args)
{
  return args->client->ft->associatorNames(args->client,
                                args->op,
                                args->assoc_class,
                                args->result_class,
                                args->role,
                                args->result_role,
                                args->status);
}

static CIMCEnumeration *
threaded_references(ruby_thread_args_t *args)
{
  return args->client->ft->references(args->client,
                                args->op,
                                args->result_class,
                                args->role,
                                args->flags, args->props, args->status);
}

static CIMCEnumeration *
threaded_reference_names(ruby_thread_args_t *args)
{
  return args->client->ft->referenceNames(args->client,
                                   args->op,
                                   args->result_class,
                                   args->role,
                                   args->status);
}

static void *
threaded_invoke_method(ruby_thread_args_t *args)
{
  CIMCData data;
  data = args->client->ft->invokeMethod(args->client,
                              args->op,
                              args->name,
                              args->argsin,
                              args->argsout,
                              args->status);
  memcpy(args->data, &data, sizeof(CIMCData));
  return NULL;
}

static void *
threaded_set_property(ruby_thread_args_t *args)
{
  CIMCStatus status;
  status = args->client->ft->setProperty(args->client, args->op, args->name, args->value, args->type);
  memcpy(args->status, &status, sizeof(CIMCStatus));
  return NULL;
}

static void *
threaded_get_property(ruby_thread_args_t *args)
{
  CIMCData data;
  data = args->client->ft->getProperty(args->client, args->op, args->name, args->status);
  memcpy(args->data, &data, sizeof(CIMCData));
  return NULL;
}
#endif

static void
dealloc(CIMCClient *c)
{
/*  fprintf(stderr, "Sfcc_dealloc_cim_client %p\n", c); */
  /* CIMCClient pointers are kept also in Enumeration
     and Instance wrappers and Ruby will try to de-allocate
     them during exit()
     So we have to prevent multiple deallocs */
  if (c && c->ft) {
    struct _CIMCClientFT *ft = c->ft;
    c->ft = NULL;
    ft->release(c);
  }
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCClass *cimclass;
  CIMCString *ops;
  rb_sfcc_object_path *rso;
  char **props;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);

  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

  props = sfcc_value_array_to_string_array(properties);
#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.flags = NUM2INT(flags);
  args.props = props;
  args.status = &status;
  cimclass = (CIMCClass *)THREAD_CALL(threaded_get_class);
#else
  cimclass = client->ft->getClass(client, rso->op, NUM2INT(flags), props, &status);
 #endif
  free(props);

  if (!status.rc) {
      return Sfcc_wrap_cim_class(cimclass);
  }
  ops = rso->op->ft->toString(rso->op, NULL);
  sfcc_rb_raise_if_error(status, "Can't get class at %s", ops->ft->getCharPtr(ops, NULL));
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCEnumeration *enm;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "11", &object_path, &flags);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.flags = NUM2INT(flags);
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_enum_class_names);
#else
  enm = client->ft->enumClassNames(client, rso->op, NUM2INT(flags), &status);
#endif
  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCEnumeration *enm;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "11", &object_path, &flags);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.flags = NUM2INT(flags);
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_enum_classes);
#else
  enm = client->ft->enumClasses(client, rso->op, NUM2INT(flags), &status);
#endif
  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCInstance *ciminstance;
  rb_sfcc_object_path *rso;
  char **props;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

  props = sfcc_value_array_to_string_array(properties);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.flags = NUM2INT(flags);
  args.props = props;
  args.status = &status;
  ciminstance = (CIMCInstance *)THREAD_CALL(threaded_get_instance);
#else
  ciminstance = client->ft->getInstance(client, rso->op, NUM2INT(flags), props, &status);
#endif
  free(props);

  if (!status.rc)
    return Sfcc_wrap_cim_instance(ciminstance, self);

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
  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCObjectPath *new_op;
  rb_sfcc_object_path *rso;
  rb_sfcc_instance *inst;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);
  Data_Get_Struct(instance, rb_sfcc_instance, inst);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.instance = inst->inst;
  args.status = &status;
  new_op = (CIMCObjectPath *)THREAD_CALL(threaded_create_instance);
#else
  new_op = client->ft->createInstance(client, rso->op, inst->inst, &status);
#endif

  if (!status.rc)
    return Sfcc_wrap_cim_object_path(new_op, self);

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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  rb_sfcc_object_path *rso;
  rb_sfcc_instance *inst;
  CIMCClient *client;
  char **props;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "22", &object_path, &instance, &flags, &properties);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);
  Data_Get_Struct(instance, rb_sfcc_instance, inst);

  props = sfcc_value_array_to_string_array(properties);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.instance = inst->inst;
  args.flags = NUM2INT(flags);
  args.props = props;
  args.status = &status;
  THREAD_CALL(threaded_set_instance);
#else
  status = client->ft->setInstance(client, rso->op, inst->inst, NUM2INT(flags), props);
#endif
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
  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCString *ops;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.status = &status;
  THREAD_CALL(threaded_delete_instance);
#else
  status = client->ft->deleteInstance(client, rso->op);
#endif
  if (status.rc) {
    ops = rso->op->ft->toString(rso->op, NULL);
    sfcc_rb_raise_if_error(status, "Can't delete instance '%s'", ops->ft->getCharPtr(ops, NULL));
  }
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
  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCEnumeration *enm;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.query = to_charptr(query);
  args.lang = to_charptr(lang);
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_exec_query);
#else
  enm = client->ft->execQuery(client,
                              rso->op,
                              to_charptr(query),
                              to_charptr(lang),
                              &status);
#endif
  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
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
  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCEnumeration *enm;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_enum_instance_names);
#else
  enm = client->ft->enumInstanceNames(client, rso->op, &status);
#endif

  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCEnumeration *enm;
  rb_sfcc_object_path *rso;
  char **props;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "12", &object_path, &flags, &properties);
  if (NIL_P(flags)) flags = INT2NUM(0);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

  props = sfcc_value_array_to_string_array(properties);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.flags = NUM2INT(flags);
  args.props = props;
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_enum_instances);
#else
  enm = client->ft->enumInstances(client, rso->op, NUM2INT(flags), props, &status);
#endif

  free(props);

  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCString *ops;
  CIMCEnumeration *enm;
  rb_sfcc_object_path *rso;
  char **props;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "16", &object_path,
               &assoc_class, &result_class,
               &role, &result_role, &flags, &properties);

  if (NIL_P(flags)) flags = INT2NUM(0);
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

  props = sfcc_value_array_to_string_array(properties);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.assoc_class = to_charptr(assoc_class);
  args.result_class = to_charptr(result_class);
  args.role = to_charptr(role);
  args.result_role = to_charptr(result_role);
  args.flags = NUM2INT(flags);
  args.props = props;
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_associators);
#else
  enm = client->ft->associators(client,
                                rso->op,
                                to_charptr(assoc_class),
                                to_charptr(result_class),
                                to_charptr(role),
                                to_charptr(result_role),
                                NUM2INT(flags), props, &status);
#endif
  free(props);
  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
  }

  ops = rso->op->ft->toString(rso->op, NULL);
  sfcc_rb_raise_if_error(status, "Can't get associators for '%s'", ops->ft->getCharPtr(ops, NULL));
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCEnumeration *enm;
  CIMCString *ops;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "14", &object_path,
               &assoc_class, &result_class,
               &role, &result_role);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.assoc_class = to_charptr(assoc_class);
  args.result_class = to_charptr(result_class);
  args.role = to_charptr(role);
  args.result_role = to_charptr(result_role);
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_associator_names);
#else
  enm = client->ft->associatorNames(client,
                                    rso->op,
                                    to_charptr(assoc_class),
                                    to_charptr(result_class),
                                    to_charptr(role),
                                    to_charptr(result_role),
                                    &status);
#endif
  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
  }
  ops = rso->op->ft->toString(rso->op, NULL);
  sfcc_rb_raise_if_error(status, "Can't get associator names for '%s'", ops->ft->getCharPtr(ops, NULL));
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCString *ops;
  CIMCEnumeration *enm;
  rb_sfcc_object_path *rso;
  char **props;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  rb_scan_args(argc, argv, "14", &object_path,
               &result_class, &role,
               &flags, &properties);

  if (NIL_P(flags)) flags = INT2NUM(0);
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

  props = sfcc_value_array_to_string_array(properties);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.result_class = to_charptr(result_class);
  args.role = to_charptr(role);
  args.flags = NUM2INT(flags);
  args.props = props;
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_references);
#else
  enm = client->ft->references(client,
                               rso->op,
                               to_charptr(result_class),
                               to_charptr(role),
                               NUM2INT(flags), props, &status);
#endif
  free(props);
  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
  }
  ops = rso->op->ft->toString(rso->op, NULL);
  sfcc_rb_raise_if_error(status, "Can't get references for '%s'", ops->ft->getCharPtr(ops, NULL));
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

  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCEnumeration *enm;
  CIMCString *ops;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif
  rb_scan_args(argc, argv, "12", &object_path,
               &result_class, &role);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.result_class = to_charptr(result_class);
  args.role = to_charptr(role);
  args.status = &status;
  enm = (CIMCEnumeration *)THREAD_CALL(threaded_reference_names);
#else
  enm = client->ft->referenceNames(client,
                                   rso->op,
                                   to_charptr(result_class),
                                   to_charptr(role),
                                   &status);
#endif
  if (enm && !status.rc ) {
    return Sfcc_wrap_cim_enumeration(enm, self);
  }
  ops = rso->op->ft->toString(rso->op, NULL);
  sfcc_rb_raise_if_error(status, "Can't get reference names for '%s'", ops->ft->getCharPtr(ops, NULL));
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
  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCArgs *cimcargsin, *cimcargsout;
  const char *method_name_cstr;
  CIMCData ret;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif
  CIMCStatus (*args_release) (CIMCArgs *);

  Check_Type(argin, T_HASH);

  cimcargsin = sfcc_hash_to_cimargs(argin);
  cimcargsout = cimcEnv->ft->newArgs(cimcEnv, NULL);

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

  method_name_cstr = to_charptr(method_name);
#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.name = method_name_cstr;
  args.argsin = sfcc_hash_to_cimargs(argin);
  args.argsout = cimcargsout;
  args.status = &status;
  args.data = &ret;
  (void)THREAD_CALL(threaded_invoke_method);
#else
  ret = client->ft->invokeMethod(client,
                              rso->op,
                              method_name_cstr,
                              cimcargsin,
                              cimcargsout,
                              &status);
#endif

  /** this is a work around the bug:
   * * https://sourceforge.net/tracker/?func=detail&aid=3555103&group_id=128809&atid=712784
   * release function has wrong declaration
   */
  args_release = (CIMCStatus (*) (CIMCArgs *)) cimcargsin->ft->release;
  args_release(cimcargsin);

  if (!status.rc) {
    if (cimcargsout && ! NIL_P(argout)) {
      Check_Type(argout, T_HASH);
      rb_funcall(argout, rb_intern("merge!"), 1, sfcc_cimargs_to_hash(cimcargsout, self));
      args_release(cimcargsout);
    }
    return sfcc_cimdata_to_value(&ret, self);
  }
  // in this case cimcargsout needs not to be freed
  // this is handled be invokeMethod
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
  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCClient *client;
  CIMCData data;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

  data = sfcc_value_to_cimdata(value);
#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.name = to_charptr(name);
  args.value = &data.value;
  args.type = data.type;
  args.status = &status;
  (void)THREAD_CALL(threaded_set_property);
#else
  status = client->ft->setProperty(client, rso->op, to_charptr(name), &data.value, data.type);
#endif

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
  CIMCClient *client;
  CIMCStatus status = {CIMC_RC_OK, NULL};
  CIMCData data;
  rb_sfcc_object_path *rso;
#if THREAD_MIGHT_BLOCK
  ruby_thread_args_t args;
#endif

  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, rb_sfcc_object_path, rso);

#if THREAD_MIGHT_BLOCK
  args.client = client;
  args.op = rso->op;
  args.name = to_charptr(name);
  args.status = &status;
  args.data = &data;
  (void)THREAD_CALL(threaded_get_property);
#else
  data = client->ft->getProperty(client, rso->op, to_charptr(name), &status);
#endif
  if ( !status.rc )
    return sfcc_cimdata_to_value(&data, self);

  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", to_charptr(name));
  return Qnil;
}

static VALUE
Sfcc_wrap_cim_client(CIMCClient *client)
{
/*  fprintf(stderr, "Sfcc_wrap_cim_client(%p)\n", client); */
  return Data_Wrap_Struct(cSfccCimClient, NULL, dealloc, client);
}

static VALUE connect(VALUE klass, VALUE host, VALUE scheme, VALUE port, VALUE user, VALUE pwd,
                     VALUE verify, VALUE trust_store, VALUE cert_file, VALUE key_file)
{
  CIMCClient *client;
  CIMCStatus status = {CIMC_RC_OK, NULL};
  /*
   * CIMCClient* (*connect2)
   * (CIMCEnv *ce, const char *hn, const char *scheme, const char *port, const char *user, const char *pwd,
   * int verifyMode, const char * trustStore,
   * const char * certFile, const char * keyFile,
   * CIMCStatus *rc);
   */

  client = cimcEnv->ft->connect2(cimcEnv,
                       to_charptr(host),
                       to_charptr(scheme),
                       to_charptr(port),
                       to_charptr(user),
                       to_charptr(pwd),
                       (verify == Qfalse)?0:1,
                       to_charptr(trust_store),
                       to_charptr(cert_file),
                       to_charptr(key_file),
                       &status);
  if ( !status.rc )
    return Sfcc_wrap_cim_client(client);
  sfcc_rb_raise_if_error(status, "Can't create CIM client");
  return Qnil;
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

  rb_define_singleton_method(klass, "native_connect", connect, 9);
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
