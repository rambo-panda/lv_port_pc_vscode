#include "./lv_canvas.h"
#include <pthread.h>

static void demo(void);

static void aa(void *_) {
  sleep(5);

  lv_obj_t *oscr = lv_scr_act();
  lv_obj_t *scr = createCanvas(1);
  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text(label, "hello second");

  sleep(3);

  lv_scr_load(oscr);
  lv_obj_del(scr);
}

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  lvCanvasReady();

  pthread_t t5;
  pthread_create(&t5, NULL, (void *)aa, NULL);

  demo();

  handlerJob();
  return 0;
}

static void demo()
{
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
  if (!lv_ft_font_init(&info))
  {
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