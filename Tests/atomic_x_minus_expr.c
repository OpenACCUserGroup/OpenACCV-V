#include "acc_testsuite.h"
#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *totals = (real_t *)malloc(10 * sizeof(real_t));
    real_t *totals_host = (real_t *)malloc(10 * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < 10; ++x){
        totals[x] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(totals[0:10])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic
                    totals[x%10] = totals[x%10] - (a[x] + b[x]);
            }
        }
    }


    for (int x = 0; x < n; ++x){
        totals_host[x%10] = totals_host[x%10] - (a[x] + b[x]);
    }
    for (int x = 0; x < 10; ++x){
        if (fabs(totals_host[x] - totals[x]) > PRECISION){
            err += 1;
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
