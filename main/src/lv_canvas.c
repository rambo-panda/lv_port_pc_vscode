#include "./lv_canvas.h"

static void ready()
{
    static int handler = 0;

    if (handler)
        return;

    handler = 1;

    LV_LOG_INFO("\r\n"
                "LVGL v%d.%d.%d "
                " Benchmark (in csv format)\r\n",
                LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    lv_init();
    LV_17_DISP_INIT;

    createDisplay();
}

static lv_disp_t *_createDefaultDisplay()
{
    // https://docs.lvgl.io/8.3/porting/display.html
    static lv_disp_draw_buf_t disp_buf;

    static lv_color_t buf_1[10 * LV_17_HOR_RES];
    static lv_color_t buf_2[10 * LV_17_HOR_RES];

    lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, 10 * LV_17_HOR_RES);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = LV_17_FLUSH_CB;
    disp_drv.hor_res = LV_17_HOR_RES;
    disp_drv.ver_res = LV_17_VER_RES;
    disp_drv.antialiasing = 1;

    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    lv_theme_t *th = lv_theme_default_init(
        disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
        LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
    lv_disp_set_theme(disp, th);
    return &disp;
}

lv_disp_t createDisplay()
{
    // lv_disp_set_default // 指定默认display
    // lv_obj_get_disp
    return *_createDefaultDisplay();
}

lv_obj_t *createCanvas(void *width, void *height)
{
    ready();

    int nowAct = width == NULL && height == NULL;

    lv_obj_t *scr = nowAct ? lv_scr_act() : lv_obj_create(NULL);

    if (!nowAct)
    {
        lv_obj_set_width(&scr, *(lv_coord_t *)width);
        lv_obj_set_height(&scr, *(lv_coord_t *)width);
        lv_scr_load(scr); /*Load the screen*/
    }

    // lv_obj_get_screen

    return scr;
}