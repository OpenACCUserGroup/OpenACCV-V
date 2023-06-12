#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,devonly,construct-independent,set,V:2.0-2.7
int test1(){
    int err = 0;

    if (acc_get_device_type() != acc_device_none){
        real_t **host_copy = (real_t **)malloc(acc_get_num_devices(acc_get_device_type()) * sizeof(real_t *));
        for (int x = 0; x < acc_get_num_devices(acc_get_device_type()); ++x){
            host_copy[x] = new real_t[n];
        }
        real_t *a = new real_t[n];
        for (int x = 0; x < acc_get_num_devices(acc_get_device_type()); ++x){
            for (int y = 0; y < n; ++y){
                a[y] = rand() / (real_t)(RAND_MAX / 10);
                host_copy[x][y] = a[y];
            }
            acc_set_device_num(x, acc_get_device_type());
            #pragma acc enter data copyin(a[0:n])
        }
        for (int x = 0; x < acc_get_num_devices(acc_get_device_type()); ++x){
            acc_set_device_num(x, acc_get_device_type());
            #pragma acc data present(a[0:n])
            {
                #pragma acc parallel
                {
                    #pragma acc loop
                    for (int y = 0; y < n; ++y){
                        a[y] = a[y] + 1;
                    }
                }
            }
        }
        for (int x = 0; x < acc_get_num_devices(acc_get_device_type()); ++x){
            acc_set_device_num(x, acc_get_device_type());
            #pragma acc exit data copyout(a[0:n])
            for (int y = 0; y < n; ++y){
                if (fabs(a[y] - (host_copy[x][y] + 1)) > PRECISION){
                    err += 1;
                }
            }
        }
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
