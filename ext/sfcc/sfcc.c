
#include "sfcc.h"

#include "cimc_environment.h"
#include "cimc_client.h"
#include "cimc_object_path.h"

VALUE mSfcc;
VALUE mSfccCimc;

void Init_sfcc()
{
  mSfcc = rb_define_module("Sfcc");
  mSfccCimc= rb_define_module_under(mSfcc, "Cimc");
  init_cimc_environment();
  init_cimc_object_path();
  init_cimc_client();
}
