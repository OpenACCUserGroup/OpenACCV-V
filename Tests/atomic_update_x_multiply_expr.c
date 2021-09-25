#include "acc_testsuite.h"
#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *totals = (real_t *)malloc((n/10 + 1) * sizeof(real_t));
    real_t *totals_comparison = (real_t *)malloc((n/10 + 1) * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < n/10 + 1; ++x){
        totals[x] = 1;
        totals_comparison[x] = 1;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(totals[0:n/10 + 1])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic update
                    totals[x%(n/10 + 1)] = totals[x%(n/10 + 1)] * (a[x] + b[x]);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        totals_comparison[x%(n/10 + 1)] *= a[x] + b[x];
    }
    for (int x = 0; x < 10; ++x){
        if (fabs(totals_comparison[x] - totals[x]) > PRECISION * totals_comparison[x]){
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
