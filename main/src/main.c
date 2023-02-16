#define _DEFAULT_SOURCE /* needed for usleep() */

#include "lvgl.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if USE_SDL == 0
#include "display/fbdev.h"
#else
#include "sdl/sdl.h"
#endif

static void hal_init(void);
static int tick_thread(void *data);
static void demo(void);

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  LV_LOG_INFO("\r\n"
              "LVGL v%d.%d.%d "
              " Benchmark (in csv format)\r\n",
              LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

  // Initialize LVGL
  lv_init();

  // Initialize the HAL (display, input devices, tick) for LVGL
  hal_init();

  demo();

  while (1)
  {
    lv_timer_handler_run_in_period(LV_DISP_DEF_REFR_PERIOD);
    usleep(LV_DISP_DEF_REFR_PERIOD * 1e3);
  }

  return 0;
}

static void demo()
{
  lv_obj_t *label2 = lv_label_create(lv_scr_act());
  lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
  lv_obj_set_width(label2, 150);
  lv_label_set_text(label2, "It is a circularly scrolling text. ");
  lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);

  // lv_obj_t *olayer = lv_scr_act();

  // {
  //   lv_obj_t *label1 = lv_label_create(olayer);
  //   lv_label_set_text(label1, "hello");
  // }

  // {
  //   lv_obj_t *scr = lv_obj_create(NULL);
  //   lv_scr_load(scr); /*Load the screen*/

  //   /*Create 2 buttons*/
  //   lv_obj_t *btn1 = lv_btn_create(scr);
  //   lv_obj_set_pos(btn1, 60, 40); /*Set the position of the button*/

  //   lv_obj_t *btn2 = lv_btn_create(btn1); /*Copy the first button*/
  //   lv_obj_set_pos(btn2, 180, 80);        /*Set the position of the button*/

  //   /*Add labels to the buttons*/
  //   lv_obj_t *label1 =
  //       lv_label_create(btn1); /*Create a label on the first button*/
  //   lv_label_set_text(label1, "Button 1"); /*Set the text of the label*/

  //   lv_obj_t *label2 =
  //       lv_label_create(btn2); /*Create a label on the second button*/
  //   lv_label_set_text(label2, "Button 2"); /*Set the text of the label*/

  //   /*Delete the second label*/
  //   lv_obj_del(label2);
  // }

  // while (1) {
  //   lv_scr_load(olayer); /*Load the screen*/
  //   break;
  // }
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
  pthread_t tickThrad;
  pthread_create(&tickThrad, NULL, (void *)tick_thread, NULL);
  pthread_detach(tickThrad);

#if USE_SDL == 1
  sdl_init();
#elif USE_FBDEV || USE_BSD_FBDEV
  fbdev_init();
#endif

  // https://docs.lvgl.io/8.3/porting/display.html
  static lv_disp_draw_buf_t disp_buf;

  static lv_color_t buf_1[SDL_HOR_RES * 10];
  static lv_color_t buf_2[SDL_HOR_RES * 10];

  lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, SDL_HOR_RES * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = SDL_HOR_RES;
  disp_drv.ver_res = SDL_VER_RES;
  disp_drv.antialiasing = 1;

  lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
}

static int tick_thread(void *data)
{
  (void)data;

  int ms = 10;

  while (1)
  {
    lv_tick_inc(ms);
    usleep(ms * 1e3);
  }

  return 0;
}
