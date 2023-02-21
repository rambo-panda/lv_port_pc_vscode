#define _GNU_SOURC
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "ffi.h"

int testFunc(int m, int n) {
    printf("Result %d", m + n );
    return m + n;
}

void ffi_test() {
    //拿函数指针
    void* functionPtr = dlsym(RTLD_DEFAULT, "testFunc");
    int argCount = 2;
  
    //按ffi要求组装好参数类型数组
    ffi_type **ffiArgTypes = alloca(sizeof(ffi_type *) *argCount);
    ffiArgTypes[0] = &ffi_type_sint;
    ffiArgTypes[1] = &ffi_type_sint;
  
    //按ffi要求组装好参数数据数组
    void **ffiArgs = alloca(sizeof(void *) *argCount);
    void *ffiArgPtr = alloca(ffiArgTypes[0]->size);
    int *argPtr = ffiArgPtr;
    *argPtr = 1;
    ffiArgs[0] = ffiArgPtr;
  
    void *ffiArgPtr2 = alloca(ffiArgTypes[1]->size);
    int *argPtr2 = ffiArgPtr2;
    *argPtr2 = 2;
    ffiArgs[1] = ffiArgPtr2;
  
    //生成 ffi_cfi 对象，保存函数参数个数/类型等信息，相当于一个函数原型
    ffi_cif cif;
    ffi_type *returnFfiType = &ffi_type_sint;
    ffi_status ffiPrepStatus = ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, (unsigned int)0, (unsigned int)argCount, returnFfiType, ffiArgTypes);
  
    if (ffiPrepStatus == FFI_OK) {
        //生成用于保存返回值的内存
        void *returnPtr = NULL;
        if (returnFfiType->size) {
            returnPtr = alloca(returnFfiType->size);
        }
        //根据cif函数原型，函数指针，返回值内存指针，函数参数数据调用这个函数
        ffi_call(&cif, functionPtr, returnPtr, ffiArgs);
  
        //拿到返回值
        int returnValue = *(int *)returnPtr;
        printf("ret: %d ", returnValue);
    }
}