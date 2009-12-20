#include "cmci_client.h"

static void
dealloc(CMCIClient *client)
{
  CMRelease(client);
}

static VALUE 
connect( VALUE klass,
         VALUE host,
         VALUE frontend,
         VALUE port,
         VALUE user,
         VALUE passwd )
{
  CMPIStatus rc;
  /* workaround for sfcb bug #2844812 */
  //memset(&rc, 0, sizeof(CMPIStatus)); 
  //if (strcmp(frontend, "SfcbLocal") != 0)
  //  frontend = "http";
  printf("1 step %s\n", StringValuePtr(host));
 
  CMCIClient *cimclient = cmciConnect(StringValuePtr(host),
                                      "SfcbLocal", //StringValuePtr(frontend),
                                      StringValuePtr(port),
                                      StringValuePtr(user),
                                      StringValuePtr(passwd),
                                      &rc);
  /*
  CMCIClient *cimclient = cmciConnect("localhost",
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL);
  */
  printf("2 step\n");

  if (cimclient == NULL) {
    rb_raise(rb_eRuntimeError, "Connection failed: %d, %s", rc.rc, rc.msg);
  } else {
    // ok, may be set cimclient->ft->ftVersion
    return Data_Wrap_Struct(klass, NULL, dealloc, cimclient);
  }
  return Qnil;
}

static VALUE each_class_name(VALUE self)
{
  CMPIStatus rc;

  CMCIClient * client;
  Data_Get_Struct(self, CMCIClient, client);

  CMPIObjectPath *op = newCMPIObjectPath("root/cimv2", "", NULL);
  CMPIEnumeration *classnames = client->ft->enumClassNames(client, op, CMPI_FLAG_DeepInheritance, &rc);

  while (classnames->ft->hasNext(classnames, NULL)) {
    CMPIData next = classnames->ft->getNext(classnames, NULL);
    char *name = CMGetCharPtr(CMObjectPathToString(next.value.ref, NULL));
    printf("%s\n", name);
  }
  CMRelease(classnames);

  if (op) CMRelease(op);
}

VALUE
Sfcc_wrap_cmci_client(VALUE klass, CMCIClient *client)
{
  return Data_Wrap_Struct(cSfccCmciClient, NULL, dealloc, client);
}

VALUE cSfccCmciClient;
void init_cmci_client()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cmci = rb_define_module_under(sfcc, "Cmci");

  VALUE klass = rb_define_class_under(cmci, "Client", rb_cObject);
  cSfccCmciClient = klass;

  rb_define_singleton_method(klass, "connect", connect, 5);
  rb_define_method(klass, "each_class_name", each_class_name, 0);

  //  cClient = rb_define_class_under(mActiveCim, "Client", rb_cObject);
  //rb_define_alloc_func( cClient, client_allocate ); 
  //rb_define_method( cClient, "initialize", client_initialize, -1 );
  //rb_define_method( cClient, "each_class_name", client_each_class_name, 0 );
}

