#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,V:1.0-2.7
int test1(){
    int err = 0;
    size_t initial_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory);
    int *a = (int *)acc_malloc(n * sizeof(int));

    if (initial_memory == 0){
        return err;
    }
    size_t final_memory = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory);
    if (final_memory + n * sizeof(int) > initial_memory){
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
