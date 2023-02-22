#define _GNU_SOURC
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <dlfcn.h>
#include "ffi.h"
#include "./lv_canvas.h"

// static const lv_obj_class_t *LV_OBJ_CLASS_ARR[] = {
//     &lv_obj_class,
//     &lv_label_class, &lv_checkbox_class, &lv_dropdown_class,
//     &lv_spangroup_class, &lv_textarea_class,
//     // lv_keyboard_class,
// };

// #define TEXT_ACTION_FN_NAME(NAME) TEXT_FOR_##NAME
// #define TEXT_ACTION_FN_PARAMS lv_obj_t *obj, char *text, int x, int y, lv_coord_t maxWidth
// #define TEXT_ACTION_FN(NAME) \
//     static const void TEXT_ACTION_FN_NAME(NAME)(TEXT_ACTION_FN_PARAMS)

// TEXT_ACTION_FN(LABEL)
// {
//     lv_label_set_text(obj, text);
//     lv_obj_set_x(obj, x);
//     lv_obj_set_x(obj, y);
//     lv_obj_set_width(obj, maxWidth);
// }
// TEXT_ACTION_FN(CHECKBOX) {}
// TEXT_ACTION_FN(DROPDOWN) {}
// TEXT_ACTION_FN(SPANGROUP) {}
// TEXT_ACTION_FN(TEXTAREA) {}

// static const void (*LV_OBJ_TEXT_PTRS[])(TEXT_ACTION_FN_PARAMS) = {
//     TEXT_ACTION_FN_NAME(LABEL),
//     TEXT_ACTION_FN_NAME(CHECKBOX),
//     TEXT_ACTION_FN_NAME(DROPDOWN),
//     TEXT_ACTION_FN_NAME(SPANGROUP),
//     TEXT_ACTION_FN_NAME(TEXTAREA),
// };

static char *METHOD_NAME_SUF = "_create";

lv_obj_t *Create(char *t, lv_obj_t *parent)
{
    if (t == "layer")
    {
        return lv_obj_create(NULL);
    }

    lv_obj_t *p = parent == NULL ? lv_scr_act() : parent;

    void *fnPtr = dlsym(RTLD_DEFAULT, GEN_FN(joinStr, METHOD_NAME_PRE, t, METHOD_NAME_SUF));

    if (fnPtr == NULL)
    {
        return NULL;
    }

    ffi_type *types[] = {&ffi_type_pointer};
    void *args[] = {&p};

    ffi_cif cif;
    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, sizeof(args) / sizeof(*args), &ffi_type_pointer, types) != FFI_OK)
    {
        return NULL;
    }

    // 生成用于保存返回值的内存
    void *returnPtr;

    ffi_call(&cif, fnPtr, &returnPtr, args);

    return (lv_obj_t *)(returnPtr);
}