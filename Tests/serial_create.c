#include "acc_testsuite.h"
#ifndef T1
//T1:serial,data,data-region,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
        c[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n]) copy(c[0:n])
    {
        #pragma acc serial create(b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = b[x];
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - c[x]) > PRECISION){
            err = 1;
        }
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int testrun;
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
