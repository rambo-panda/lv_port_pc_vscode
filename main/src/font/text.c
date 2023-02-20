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


static void *fillText(const char *text, const lv_obj_t *parent, const lv_obj_t *obj, int *x, int *y,  int *maxWidth) {
    if (text == NULL) {
        return NULL;
    }

    lv_obj_t *_parent = parent == NULL ? lv_scr_act() : parent;
    lv_obj_t *o = obj == NULL ? lv_spangroup_create(_parent) : o;
    lv_span_t * span = lv_spangroup_new_span(o);

    // 在这个容器内的x y
    // if (x != NULL) {
    //     lv_obj_set_x(o, *(lv_coord_t *)x);
    // }

    // if (y != NULL) {
    //     lv_obj_set_y(o, *(lv_coord_t *)y);
    // }
}