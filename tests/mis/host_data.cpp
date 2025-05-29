#include "acc_testsuite.h"
#ifndef T1
//T1:data,data-region,host-data,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    int * high = new int[n];
    int high_current_index = 0;
    size_t * a_points = (size_t *)malloc(n * sizeof(void *));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        if (a[x] > 5) {
            high[high_current_index] = x;
            high_current_index += 1;
        }
    }

    #pragma acc enter data copyin(a[0:n])

    #pragma acc host_data use_device(a)
    {
        int x = 0;
        for (x = 0; x < high_current_index; ++x){
            a_points[x] = (size_t) a + (high[x]*sizeof(real_t*));
        }
        for (; x < n; ++x){
            a_points[x] = 0;
        }
    }
    #pragma acc enter data copyin(a_points[0:n])
    #pragma acc parallel present(a[0:n], a_points[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            if (a_points[x] != 0){
                *((real_t *) a_points[x]) -= 5;
            }
        }
    }
    #pragma acc exit data delete(a_points[0:n]) copyout(a[0:n])
    for (int x = 0; x < n; ++x){
        if (a[x] < 0 || a[x] > 5) {
            err += 1;
            break;
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
