#define _GNU_SOURC
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <stdio.h>
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

static char *methodNamePre = "lv_";
static char *methodNameSuf = "_create";

static char *joinStr(int a, ...) {
    if (a <= 1) {
        return "";
    }

    ARGS(a, char *);

    char *str = (char *)malloc(1);

    for(int i = 0; i < a; i++) {
        str = (char *) realloc(str, strlen(str) + strlen(args[i]));
        strcat(str, args[i]);
    }

    return str;
}

lv_obj_t *Create(char *t, lv_obj_t *parent)
{
    if (t == "layer")
    {
        return lv_obj_create(NULL);
    }


    lv_obj_t *p = parent == NULL ? lv_scr_act() : parent;

    void *functionPtr = dlsym(RTLD_DEFAULT, GEN_FN(joinStr, methodNamePre, t, methodNameSuf));

    int argCount = 1;
    ffi_type **ffiArgTypes = alloca(sizeof(ffi_type *) * argCount);
    ffiArgTypes[0] = &ffi_type_pointer;

    void **ffiArgs = alloca(sizeof(void *) *argCount);
    ffiArgs[0] = &p;

    ffi_cif cif;
    ffi_type *returnFfiType = &ffi_type_pointer;
    ffi_status ffiPrepStatus = ffi_prep_cif_var(
        &cif,
        FFI_DEFAULT_ABI,
        (unsigned int)0,
        (unsigned int)argCount,
        returnFfiType,
        ffiArgTypes);

    lv_obj_t *ret = NULL;

    if (ffiPrepStatus == FFI_OK)
    {
        // 生成用于保存返回值的内存
        void *returnPtr = NULL;
        if (returnFfiType->size)
        {
            returnPtr = alloca(returnFfiType->size);
        }
        ffi_call(&cif, functionPtr, returnPtr, ffiArgs);

        // 拿到返回值
        ret = (lv_obj_t *)returnPtr;
    }

    return ret;
}