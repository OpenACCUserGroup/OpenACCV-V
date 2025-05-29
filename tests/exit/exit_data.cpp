#include "acc_testsuite.h"
#ifndef T1
//T1:data,executable-data,devonly,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * a_copy = new real_t[n];
    int * devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            a_copy[x] = a[x];
        }

        #pragma acc enter data copyin(a[0:n])
        #pragma acc parallel present(a[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                a[x] = a[x] + 1;
            }
        }
        #pragma acc exit data delete(a[0:n])

        for (int x = 0; x < n; ++x){
            if (fabs(a[x] - a_copy[x]) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:data,executable-data,V:2.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * a_copy = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        a_copy[x] = a[x];
    }

    #pragma acc enter data copyin(a[0:n])
    #pragma acc parallel present(a[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            a[x] = a[x] + 1;
        }
    }
    #pragma acc exit data copyout(a[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - (a_copy[x] + 1)) > PRECISION){
            err += 1;
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
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test2();
    }
    if (failed != 0){
        failcode = failcode + (1 << 1);
    }
#endif
    return failcode;
}
