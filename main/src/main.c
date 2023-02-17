#define _DEFAULT_SOURCE /* needed for usleep() */

#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if USE_SDL == 0
#include "display/fbdev.h"
#else
#include "sdl/sdl.h"
#endif

static void hal_init(void);
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
    lv_timer_handler_run_in_period(LV_DISP_DEF_REFR_PERIOD);
    usleep(LV_DISP_DEF_REFR_PERIOD * 1e3);
  }

  return 0;
}

static void demo() {
  lv_obj_t *img = lv_img_create(lv_scr_act());

  // lv_img_set_src(img, "S:lvgl/examples/libs/png/wink.png");
  // lv_img_set_src(img, "S:lvgl/examples/libs/png/aaa.png");
  lv_obj_set_width(img, LV_SIZE_CONTENT);
  lv_obj_set_height(img, LV_SIZE_CONTENT);
  // LV_IMG_CF_TRUE_COLOR_ALPHA
  lv_img_set_src(img, "S:lvgl/examples/libs/png/a.png");
  // lv_img_set_src(img, "S:xxx.png");

  lv_obj_center(img);

  // lv_obj_t * img2 = lv_img_create(lv_scr_act());
  // lv_img_set_src(img2, "S:lvgl/examples/libs/png/wink.png");
  // lv_obj_align(img2, LV_ALIGN_RIGHT_MID, -20, 0);

  return;
  static lv_ft_info_t info;
  info.name = "./lvgl/examples/libs/freetype/iconfont.ttf";
  info.weight = 16;
  info.style = FT_FONT_STYLE_NORMAL;
  info.mem = NULL;
  if (!lv_ft_font_init(&info)) {
    LV_LOG_ERROR("create failed.");
  }

  /*Create style with the new font*/
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, info.font);
  // lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

  /*Create a label with the new style*/
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_obj_add_style(label, &style, 0);
  // https://onlineunicodetools.com/convert-unicode-to-utf8
  // lv_label_set_text(label, "ic_保存 -> \nic_加号 -> \nic_开始播放 ->
  // \nic_正在删除 -> \nic_保存 -> \nic_开始 -> \nic_警告 ->
  // \nic_停止 -> \nic_数据上传 -> \nic_battery_20%以下 ->
  // \nic_上传结束 -> \nic_volume_11-15 -> \nic_成功 ->
  // \nic_volume_6-10 -> \nic_battery_充电中 -> \nic_加载 ->
  // \nic_wifi_未开启 -> \nic_上传异常 -> \nic_pen_断开 ->
  // \nic_pen_充电中 -> \nic_battery_60% -> \nic_暂停 ->
  // \nic_pen_链接-电量正常 -> \nic_battery_full -> \nic_battery_40% ->
  // \nic_wifi_1 -> \nic_wifi_开启异常 -> \nic_pen_更新中 ->
  // \nic_pen_链接-电量低 -> \nic_wifi_未连接 -> \nic_battery_80% ->
  // \nic_volume_1-5 -> \nic_wifi_2 -> \nic_volume_0 -> \nic_wifi_3
  // -> \nic_battery_充电中 -> \nic_battery_80% -> ");
  lv_label_set_text(label, "ic_保存 -> \nic_加号 -> ");
  lv_obj_center(label);
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void) {
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

  lv_theme_t *th = lv_theme_default_init(
      disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
      LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);
}
