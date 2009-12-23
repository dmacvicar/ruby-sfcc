#include "cimc_environment.h"
#include "cimc_object_path.h"
#include "cimc_client.h"

static void
dealloc(CIMCEnv *env)
{
  ReleaseCIMCEnv(env);
}

/**
 * call-seq:
 *  connect(host, frontend, port, user, passwd)
 *
 * Connect to the CIMOM on +host+ and +port+ identified by +user+
 * and +passwd+.
 *
 * Use "http" for +frontend+
 */
static VALUE 
connect( VALUE self,
         VALUE host,
         VALUE frontend,
         VALUE port,
         VALUE user,
         VALUE passwd )
{
  CIMCEnv *env = NULL;
  CIMCStatus rc;
  CIMCClient *client = NULL;
  char *msg = NULL;
  VALUE rb_client;

  // workaround for sfcb bug #2844812
  memset(&rc, 0, sizeof(CIMCStatus));
  
  Data_Get_Struct(self, CIMCEnv, env);
  client = env->ft->connect(env,
                            StringValuePtr(host),
                            StringValuePtr(frontend),
                            StringValuePtr(port),
                            StringValuePtr(user),
                            StringValuePtr(passwd),
                            &rc);
  if(client && !rc.rc) {
    rb_client = Sfcc_wrap_cimc_client(client);
    if (rb_block_given_p()) rb_yield(rb_client);
    return rb_client;
  }

  rb_raise(rb_eRuntimeError, "Can't connect to CIMOM: %s", msg);
  return Qnil;
}

/**
 * call-seq:
 *  new(env_id)
 *
 * Creates a new CIM environment
 * +env_id+ "SfcbLocal" for local communication to the
 * CIMOM, or "XML" for talking to it via xml.
 */
static VALUE new(VALUE klass, VALUE env_id)
{
  int rc = 0;
  char *msg;
  CIMCEnv *env = NewCIMCEnv(StringValuePtr(env_id), 0, &rc, &msg);

  if (env && !rc) {
    VALUE rb_env = Sfcc_wrap_cimc_environment(env) ;
    rb_obj_call_init(rb_env, 0, NULL);

    if(rb_block_given_p()) rb_yield(rb_env);
    return rb_env;
  }
  
  rb_raise(rb_eRuntimeError, "(%d) Can't create CIM environment: %s", rc, msg);
  return Qnil;
}

/**
 * call-seq:
 *  new_object_path(namespace, classname)
 *
 * Creates a new object path for +namespace+ and +classname+
 */
static VALUE new_object_path(VALUE self,
                             VALUE namespace,
                             VALUE classname)
{
  CIMCEnv *env = NULL;
  CIMCObjectPath *op = NULL;
  CIMCStatus status;
  Data_Get_Struct(self, CIMCEnv, env);

  op = env->ft->newObjectPath(env,
                              StringValuePtr(namespace),
                              StringValuePtr(classname),
                              &status);
  if (!status.rc) {
    return Sfcc_wrap_cimc_object_path(op);
  }
  sfcc_rb_raise_if_error(status, "Can't create CIM object path");
  return Qnil;
  
}

VALUE
Sfcc_wrap_cimc_environment(CIMCEnv *environment)
{
  assert(environment);
  return Data_Wrap_Struct(cSfccCimcEnvironment, NULL, dealloc, environment);
}

VALUE cSfccCimcEnvironment;
void init_cimc_environment()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  VALUE klass = rb_define_class_under(cimc, "Environment", rb_cObject);
  cSfccCimcEnvironment = klass;

  rb_define_singleton_method(klass, "new", new, 1);
  rb_define_method(klass, "connect", connect, 5);
  rb_define_method(klass, "new_object_path", new_object_path, 2);
}

