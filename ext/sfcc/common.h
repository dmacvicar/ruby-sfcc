
#ifndef _COMMON_H
#define _COMMON_H

// make the compiler happy when
// calling rb_define_method()
typedef VALUE (ruby_method)(...);
// more useful macros
#define RB_FINALIZER(func) ((void (*)(...))func)

// this macro saves us from typing
// (ruby_method*) & method_name
// in rb_define_method
#define RB_METHOD(func) ((VALUE (*)(...))func)

#endif
