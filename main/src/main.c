#define _DEFAULT_SOURCE /* needed for usleep() */

#include "lvgl.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#if USE_MONITOR == 0
#include "display/fbdev.h"
#else
#define SDL_MAIN_HANDLED // To fix SDL's "undefined reference to WinMain"
                         // issue
#include "display/monitor.h"
#include <SDL2/SDL.h>
#endif

static void hal_init(void);
static int tick_thread(void *data);
static void demo(void);

int main(int argc, char **argv) {
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

  while (1) {
    int ms = LV_DISP_DEF_REFR_PERIOD * 1e3;
    lv_timer_handler_run_in_period(ms);
    usleep(ms);
  }

  return 0;
}

static void demo() {
  lv_obj_t *olayer = lv_scr_act();

  {
    lv_obj_t *label1 = lv_label_create(olayer);
    lv_label_set_text(label1, "hello");
  }

  {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_scr_load(scr); /*Load the screen*/

    /*Create 2 buttons*/
    lv_obj_t *btn1 = lv_btn_create(scr);
    lv_obj_set_pos(btn1, 60, 40); /*Set the position of the button*/

    lv_obj_t *btn2 = lv_btn_create(btn1); /*Copy the first button*/
    lv_obj_set_pos(btn2, 180, 80);        /*Set the position of the button*/

    /*Add labels to the buttons*/
    lv_obj_t *label1 =
        lv_label_create(btn1); /*Create a label on the first button*/
    lv_label_set_text(label1, "Button 1"); /*Set the text of the label*/

    lv_obj_t *label2 =
        lv_label_create(btn2); /*Create a label on the second button*/
    lv_label_set_text(label2, "Button 2"); /*Set the text of the label*/

    /*Delete the second label*/
    lv_obj_del(label2);
  }

  while (1) {
    lv_scr_load(olayer); /*Load the screen*/
    break;
  }
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void) {
/* Use the 'monitor' driver which creates window on PC's monitor to simulate a
 * display*/
#if USE_MONITOR == 1
  monitor_init();
#elif USE_FBDEV || USE_BSD_FBDEV
  fbdev_init();
#endif

  pthread_t tickThrad;
  pthread_create(&tickThrad, NULL, (void *)tick_thread, NULL);
  pthread_detach(tickThrad);

#if USE_MONITOR == 1
  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
  static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);
#endif

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
#if USE_MONITOR == 1
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = monitor_flush;
  disp_drv.hor_res = MONITOR_HOR_RES;
  disp_drv.ver_res = MONITOR_VER_RES;
#elif USE_FBDEV || USE_BSD_FBDEV
  disp_drv.flush_cb = fbdev_flush;
#endif
  disp_drv.antialiasing = 1;

  // register display
  lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

  lv_theme_t *th = lv_theme_default_init(
      disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
      LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);
}

static int tick_thread(void *data) {
  (void)data;

  int ms = 10 * 1e3;

  while (1) {
    lv_tick_inc(ms);
    usleep(ms);
  }

  return 0;
}
