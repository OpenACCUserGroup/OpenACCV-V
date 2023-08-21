#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,construct-independent,V:3.3
int test1(){
    int err = 0;
    srand(SEED);
    two_d_array data;
    
    data.a = (real_t *)malloc(n * sizeof(real_t));
    data.b = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        data.a[x] = rand() / (real_t)(RAND_MAX / 10);
        data.b[x] = 2 * data.a[x];
    }

    #pragma acc enter data copyin(data.a[0:n], data.b[0:n])
    #pragma acc enter data copyin(data)
    acc_attach(&data.a);
    acc_attach(&data.b);

    #pragma acc parallel loop default(present)
    for(int x = 0; x < n; ++x){
        data.a[x] = data.a[x] * 2;
    }

    #pragma acc exit data detach(data.a, data.b)
    #pragma acc exit data copyout(data.a[0:n], data.b[0:n])
    #pragma acc exit data copyout(data)

    for (int x = 0; x < n; ++x){
        if (fabs(data.a[x] - data.b[x]) > PRECISION){
            err += 1;
        }
    }

    free(data.a);
    free(data.b);

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
