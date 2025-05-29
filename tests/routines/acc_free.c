#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,V:2.6-2.7
int test1(){
    int err = 0;
    int *a = (int *)acc_malloc(n * sizeof(int));
    size_t initial_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory);

    acc_free(a);
    size_t final_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory);
    if (final_memory < initial_memory + n * sizeof(int)){
        err += 1;
    }
    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test1();
    }
    if (failed != 0){
        failcode = failcode + (1 << 0);
    }
#endif
    return failcode;
}
