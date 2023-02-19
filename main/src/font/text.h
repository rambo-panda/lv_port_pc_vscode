#include <stdarg.h>
#include "lvgl.h"

#define COUNT_ARGS(...) (sizeof((int[]){0, ##__VA_ARGS__}) / sizeof(int) - 1)

#define GEN_FN(func, ...) (func(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__))
// #define gen_fn(func, ...) (#func(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__))

#define ARGS(count, __type)                                                    \
  __type args[count];                                                          \
  va_list _args;                                                               \
  va_start(_args, count);                                                      \
  for (int X_I = 0; X_I < count; X_I++) {                                      \
    args[X_I] = va_arg(_args, __type);                                         \
  }                                                                            \
  va_end(_args)