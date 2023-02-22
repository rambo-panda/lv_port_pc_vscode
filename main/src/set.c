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

static char *METHOD_NAME_TAG = "_set_";

// static char *ss[] = {"fsdfds", "fdfsd", "fdsfsd"};
// static void *SetObj()
// char *s[] = {
// };
// char x[] = {"fdsfds"};
// char x[5] = "fdsfd";

void Set(char *action, lv_obj_t *parent, char *oName, int len, ...)
{
    lv_obj_t *p = parent == NULL ? lv_scr_act() : parent;

    char *_o = (oName == NULL) ? "obj" : oName;
    void *functionPtr = dlsym(RTLD_DEFAULT, GEN_FN(joinStr, METHOD_NAME_PRE, _o, METHOD_NAME_TAG, action));
    free(_o);

    ARGS(len, void *);

    ffi_type **ffiArgTypes = alloca(sizeof(ffi_type *) * (len + 1));
    ffiArgTypes[0] = &p;
    for (int i = 0; i < len; i++)
    {
        ffiArgTypes[i] = &ffi_type_pointer;
    }

    // void **ffiArgs = alloca(sizeof(void *) * argCount);
    // ffiArgs[0] = &p;

    // ffi_cif cif;
    // ffi_type *returnFfiType = &ffi_type_pointer;
    // ffi_status ffiPrepStatus = ffi_prep_cif_var(
    //     &cif,
    //     FFI_DEFAULT_ABI,
    //     (unsigned int)0,
    //     (unsigned int)argCount,
    //     returnFfiType,
    //     ffiArgTypes);

    // lv_obj_t *ret = NULL;

    // if (ffiPrepStatus == FFI_OK)
    // {
    //     // 生成用于保存返回值的内存
    //     void *returnPtr = NULL;
    //     if (returnFfiType->size)
    //     {
    //         returnPtr = alloca(returnFfiType->size);
    //     }
    //     ffi_call(&cif, functionPtr, returnPtr, ffiArgs);

    //     // 拿到返回值
    //     ret = (lv_obj_t *)returnPtr;
    // }

    // return ret;
}