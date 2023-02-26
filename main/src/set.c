#define _GNU_SOURC
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <dlfcn.h>
#include "ffi.h"
#include "./lv_canvas.h"

// static char *METHOD_NAME_TAG = "_set_";

// static void *(*genTypeFns[10])(int, ...);
// static void *(*doSetFns[10])(int, ...);

// int main(int argc, char *argv[])
// {
 
//     //使用函数类型指针方式调用
//     FUNC_HELLO_PTR *t = hello;
//     t();
//     //直接使用函数指针调用
//     hellotest = hello;
//     hellotest();
 
//     return 0;
// }


// void Set(char *action, lv_obj_t *parent, char *oName, int len, ...)
// {
//     // lv_obj_t *p = parent == NULL ? lv_scr_act() : parent;
// }
