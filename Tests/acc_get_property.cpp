#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,devonly,syntactic,V:2.6-2.7
int test1(){
    int err = 0;

    if (acc_get_device_type() != acc_device_none){
        const char* returned_string1;
        const char* returned_string2;
        const char* returned_string3;
        int returned_int;
        returned_int = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_memory);
        returned_int = acc_get_property(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_free_memory);
        returned_string1 = acc_get_property_string(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_name);
        returned_string2 = acc_get_property_string(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_vendor);
        returned_string3 = acc_get_property_string(acc_get_device_num(acc_get_device_type()), acc_get_device_type(), acc_property_driver);
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
