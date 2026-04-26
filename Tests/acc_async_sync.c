// acc_async_sync.c
//
// Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
// Compute constructs were updated to use the async-argument and
// wait-argument grammar consistently with the rest of the specification.
//
// Test:
// T1 – Explicit acc_async_sync: Uses async(acc_async_sync).
// T2 – Behavior check: verifies no async clause behaves like async(acc_async_sync).

#include "acc_testsuite.h"
#include <openacc.h>
#include <math.h>
#include <stdlib.h>

#ifndef T1
//T1:async-argument,special-value,compute-constructs,acc_async_sync,V:3.4-
int test1() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c){
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        #pragma acc parallel loop async(acc_async_sync)
        for (int i = 0; i < n; ++i){
            c[i] = a[i] + b[i];
        }

        #pragma acc wait
    }

    for (int i = 0; i < n; ++i) {
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION){
            err++;
        }
    }

    free(a);
    free(b);
    free(c);
    return err;
}
#endif

#ifndef T2
//T2:async-argument,special-value,compute-constructs,acc_async_sync,equivalence,V:3.4-
int test2() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    real_t *d = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c || !d){
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
        d[i] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n], d[0:n])
    {
        /* No async clause: should behave as if async argument is acc_async_sync */
        #pragma acc parallel loop
        for (int i = 0; i < n; ++i){
            c[i] = a[i] + b[i];
        }

        /* Explicit acc_async_sync form */
        #pragma acc parallel loop async(acc_async_sync)
        for (int i = 0; i < n; ++i){
            d[i] = a[i] + b[i];
        }
    }

    for (int i = 0; i < n; ++i){
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION){
            err++;
        }
        if (fabs(d[i] - (a[i] + b[i])) > PRECISION){
            err++;
        }
        if (fabs(c[i] - d[i]) > PRECISION){
            err++;
        }
    }

    free(a);
    free(b);
    free(c);
    free(d);
    return err;
}
#endif

int main() {
    int failcode = 0;
    int failed;

#ifndef T1
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test1();
    }
    if (failed){
        failcode |= (1 << 0);
    }
#endif

#ifndef T2
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test2();
    }
    if (failed){
        failcode |= (1 << 1);
    }
#endif

    return failcode;
}
