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

  CIMCEnumeration *enm = client->ft->enumClassNames(client, op, CIMC_FLAG_DeepInheritance, &rc);
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

