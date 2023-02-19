#include "./text.h"

static const lv_obj_class_t *LV_OBJ_CLASS_ARR[] = {
    &lv_label_class, &lv_checkbox_class, &lv_dropdown_class,
    &lv_spangroup_class, &lv_textarea_class,
    // lv_keyboard_class,
};

#define TEXT_ACTION_FN_NAME(NAME) TEXT_FOR_##NAME
#define TEXT_ACTION_FN_PARAMS lv_obj_t *obj, char *text, int x, int y, lv_coord_t maxWidth
#define TEXT_ACTION_FN(NAME) \
    static const void TEXT_ACTION_FN_NAME(NAME)(TEXT_ACTION_FN_PARAMS)

TEXT_ACTION_FN(LABEL)
{
    lv_label_set_text(obj, text);
    lv_obj_set_x(obj, x);
    lv_obj_set_x(obj, y);
    lv_obj_set_width(obj, maxWidth);
}
TEXT_ACTION_FN(CHECKBOX) {}
TEXT_ACTION_FN(DROPDOWN) {}
TEXT_ACTION_FN(SPANGROUP) {}
TEXT_ACTION_FN(TEXTAREA) {}

static const void (*LV_OBJ_TEXT_PTRS[])(TEXT_ACTION_FN_PARAMS) = {
    TEXT_ACTION_FN_NAME(LABEL),
    TEXT_ACTION_FN_NAME(CHECKBOX),
    TEXT_ACTION_FN_NAME(DROPDOWN),
    TEXT_ACTION_FN_NAME(SPANGROUP),
    TEXT_ACTION_FN_NAME(TEXTAREA),
};

//static void *fillText(char *text, int *x, int *y, lv_obj_t *obj, lv_coord_t *maxWidth)
static void *fillText(char *text, int *x, int *y, lv_obj_t *obj, lv_coord_t *maxWidth)
{
    lv_obj_t *parent = obj == NULL ? lv_label_create(lv_scr_act()) : obj;
    // lv_label_set_text();
    // lv_label_class
    // switch (lv_obj_get_class(parent))
    int len = sizeof LV_OBJ_CLASS_ARR / sizeof *LV_OBJ_CLASS_ARR;
    while (--len > -1)
    {
        if (parent->class_p == LV_OBJ_CLASS_ARR[len])
        {
            (*LV_OBJ_TEXT_PTRS[len])(parent, text, x, y, maxWidth);
            break;
        }
    }
}
