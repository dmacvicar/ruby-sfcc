
#include <stdarg.h>
#include "sfcc.h"

#include "cimc_environment.h"
#include "cimc_object_path.h"
#include "cimc_class.h"
#include "cimc_client.h"

VALUE mSfcc;
VALUE mSfccCimc;

void Init_sfcc()
{
  mSfcc = rb_define_module("Sfcc");
  mSfccCimc= rb_define_module_under(mSfcc, "Cimc");

  init_cimc_environment();
  init_cimc_object_path();
  init_cimc_class();
  init_cimc_client();
}

static VALUE sfcc_status_exception(CIMCStatus status)
{
  switch (status.rc)
  {
  case CIMC_RC_ERR_FAILED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorFailed"));
  case CIMC_RC_ERR_ACCESS_DENIED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorAcessDenied"));
  case CIMC_RC_ERR_INVALID_NAMESPACE:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidNamespace"));
  case CIMC_RC_ERR_INVALID_PARAMETER:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidParameter"));
  case CIMC_RC_ERR_INVALID_CLASS:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidClass"));
  case CIMC_RC_ERR_NOT_FOUND:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNotFound"));
  case CIMC_RC_ERR_NOT_SUPPORTED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNotSupported"));
  case CIMC_RC_ERR_CLASS_HAS_CHILDREN:
    return rb_const_get(mSfccCimc, rb_intern("ErrorClassHasChildren"));
  case CIMC_RC_ERR_CLASS_HAS_INSTANCES:
    return rb_const_get(mSfccCimc, rb_intern("ErrorClassHasInstances"));
  case CIMC_RC_ERR_INVALID_SUPERCLASS:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidSuperClass"));
  case CIMC_RC_ERR_ALREADY_EXISTS:
    return rb_const_get(mSfccCimc, rb_intern("ErrorAlreadyExists"));
  case CIMC_RC_ERR_NO_SUCH_PROPERTY:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNoSuchProperty"));
  case CIMC_RC_ERR_TYPE_MISMATCH:
    return rb_const_get(mSfccCimc, rb_intern("ErrorTypeMismatch"));
  case CIMC_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED:
    return rb_const_get(mSfccCimc, rb_intern("ErrorQueryLanguageNotSupported"));
  case CIMC_RC_ERR_INVALID_QUERY:
    return rb_const_get(mSfccCimc, rb_intern("ErrorInvalidQuery"));
  case CIMC_RC_ERR_METHOD_NOT_AVAILABLE:
    return rb_const_get(mSfccCimc, rb_intern("ErrorMethodNotAvailable"));
  case CIMC_RC_ERR_METHOD_NOT_FOUND:
    return rb_const_get(mSfccCimc, rb_intern("ErrorMethodNotFound"));
  case CIMC_RC_DO_NOT_UNLOAD:
    return rb_const_get(mSfccCimc, rb_intern("ErrorDoNotUnload"));
  case CIMC_RC_NEVER_UNLOAD:
    return rb_const_get(mSfccCimc, rb_intern("ErrorNeverUnload"));
  case CIMC_RC_ERROR_SYSTEM:
    return rb_const_get(mSfccCimc, rb_intern("ErrorSystem"));
  case CIMC_RC_ERROR:
    return rb_const_get(mSfccCimc, rb_intern("ErrorRcError"));
  case CIMC_RC_OK:
  default:
    return Qnil;
  }
}


void sfcc_rb_raise_if_error(CIMCStatus status, const char *msg, ...)
{
  va_list arg_list;
  va_start(arg_list, msg);

  if (!status.rc)
    return;

  rb_raise(sfcc_status_exception(status), msg, arg_list);
}
