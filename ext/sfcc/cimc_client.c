#include "cimc_client.h"
#include "cimc_object_path.h"

static void
dealloc(CIMCClient *client)
{
  client->ft->release(client);
}

/*
 * call-seq:
 *  each_class_name(object_path, flags)
 *
 * Enumerate the available class names for the given
 * +object_path+ and +flags+
 */
static VALUE each_class_name(VALUE self, VALUE object_path, VALUE flags)
{
  CIMCStatus rc;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  CIMCString *path = NULL;

  memset(&rc, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);

  CIMCEnumeration *enm = client->ft->enumClassNames(client, op, NUM2INT(flags), &rc);
  while (enm->ft->hasNext(enm, NULL)) {
    CIMCData next = enm->ft->getNext(enm, NULL);
    op = next.value.ref;
    path = op->ft->toString(op, NULL);
    char *name = path->ft->getCharPtr(path, NULL);
    rb_yield(rb_str_new2(name));
  }
  CMRelease(enm);
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
  CIMCStatus rc;
  CIMCObjectPath *op = NULL;
  CIMCClient *client = NULL;
  CIMCString *path = NULL;
  CIMCClass *cimclass = NULL;

  memset(&rc, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCClient, client);
  Data_Get_Struct(object_path, CIMCObjectPath, op);


  cimclass = client->ft->getClass(client, op, NUM2INT(flags), properties, &rc);
  return Qnil;
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
}

