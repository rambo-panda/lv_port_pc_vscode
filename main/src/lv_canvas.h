#ifndef LV_CANVAS_LIB
#define LV_CANVAS_LIB
#define _DEFAULT_SOURCE /* needed for usleep() */

#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define METHOD_NAME_PRE  "lv_"

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

lv_obj_t *createCanvas(int nowAct);
void lvCanvasReady();
static inline void handlerJob()
{
    while (1)
    {
        lv_timer_handler_run_in_period(LV_DISP_DEF_REFR_PERIOD);
        usleep(LV_DISP_DEF_REFR_PERIOD * 1e3);
    }
};

lv_obj_t *Create(char *t, lv_obj_t *parent);
char *joinStr(int a, ...);

#if USE_SDL == 1
#include "sdl/sdl.h"
#define LV_17_DISP_INIT sdl_init()
#define LV_17_VER_RES SDL_VER_RES
#define LV_17_HOR_RES SDL_HOR_RES
#define LV_17_FLUSH_CB sdl_display_flush
#elif USE_FBDEV || USE_BSD_FBDEV
#include "display/fbdev.h"
#define LV_17_DISP_INIT fbdev_init()
#define LV_17_HOR_RES LV_HOR_RES
#define LV_17_VER_RES LV_VER_RES
#define LV_17_FLUSH_CB fbdev_flush
#endif

#endif
