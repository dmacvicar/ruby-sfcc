#include "cimc_client.h"
#include "cimc_object_path.h"
#include "cimc_class.h"

static void
dealloc(CIMCClient *client)
{
  client->ft->release(client);
}

/**
 * call-seq:
 *  each_class_name(object_path, flags)
 *
 * Enumerate the available class names for the given
 * +object_path+ and +flags+
 */
static VALUE each_class_name(VALUE self, VALUE object_path, VALUE flags)
{
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  CIMCString *path = NULL;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  CIMCEnumeration *enm = client->ft->enumClassNames(client, op, NUM2INT(flags), &status);

  if (enm && !status.rc ) {
    while (enm->ft->hasNext(enm, NULL)) {
      CIMCData next = enm->ft->getNext(enm, NULL);
      op = next.value.ref;
      path = op->ft->toString(op, NULL);
      char *name = path->ft->getCharPtr(path, NULL);
      rb_yield(rb_str_new2(name));
    }
    enm->ft->release(enm);
    return Qnil;
  }
  sfcc_rb_raise_if_error(status, "Can't get class names");
  return Qnil;
}

/**
 * call-seq:
 *  each_instance_name(object_path, flags)
 *
 * Enumerate the instance names of the class defined by +object_path+
 * +object_path+
 */
static VALUE each_instance_name(VALUE self, VALUE object_path)
{
  CIMCStatus status;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  CIMCString *path = NULL;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  CIMCEnumeration *enm = client->ft->enumInstanceNames(client, op, &status);

  if (enm && !status.rc ) {
    while (enm->ft->hasNext(enm, NULL)) {
      CIMCData next = enm->ft->getNext(enm, NULL);
      op = next.value.ref;
      path = op->ft->toString(op, NULL);
      char *name = path->ft->getCharPtr(path, NULL);
      rb_yield(rb_str_new2(name));
    }
    enm->ft->release(enm);
    return Qnil;
  }

  sfcc_rb_raise_if_error(status, "Can't get instance names");
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
  char *props[RARRAY_LEN(properties)];
  int i = 0;

  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  for (; i < RARRAY_LEN(properties); ++i)
    props[i] = StringValuePtr(*(RARRAY_PTR(properties) + i));

  cimclass = client->ft->getClass(client, op, NUM2INT(flags), props, &status);
  sfcc_rb_raise_if_error(status, "Can't get class");
  return Sfcc_wrap_cimc_class(cimclass);
}

VALUE
Sfcc_wrap_cimc_client(CIMCClient *client)
{
  assert(client);
  return Data_Wrap_Struct(cSfccCimcClient, NULL, dealloc, client);
}

VALUE cSfccCimcClient;
void init_cimc_client()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cimc");

  VALUE klass = rb_define_class_under(cimc, "Client", rb_cObject);
  cSfccCimcClient = klass;

  rb_define_method(klass, "each_class_name", each_class_name, 2);
  rb_define_method(klass, "each_instance_name", each_instance_name, 1);
  rb_define_method(klass, "get_class", get_class, 3);
}

