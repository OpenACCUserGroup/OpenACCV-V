// acc_async_argument.c
//
// Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
// Compute constructs were updated to use the async-argument and
// wait-argument grammar consistently with the rest of the specification
//
// Tests:
// T1 – Basic async usage: Runs a parallel loop using async(q0) where q0
//      is an integer expression, followed by wait with no argument.
// T2 – Default async queue: Uses async with no argument after setting
//      the default queue with acc_set_default_async().
// T3 – Explicit acc_async_sync: Uses async(acc_async_sync).
// T4 – Explicit acc_async_noval: Uses async(acc_async_noval).

#include "acc_testsuite.h"
#include <openacc.h>
#include <math.h>
#include <stdlib.h>

#ifndef T1
//T1:async,wait,runtime,compute-constructs,baseline,V:3.4-
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

    int q = 4;
    int q0 = q - q; /* int-expr evaluating to 0 => nonnegative scalar integer expression */

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n]) async(q0)
        for (int i = 0; i < n; ++i) {
            c[i] = a[i] + b[i];
        }

        /* wait with no argument => wait-all activity queues (spec-valid) */
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
int test2() {
    int err = 0;
    srand(SEED);

    const int q = 7;
    acc_set_default_async(q);

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

    #pragma acc enter data copyin(a[0:n], b[0:n]) create(c[0:n])

    /* async (no arg) => default async queue */
    #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n]) async
    for (int i = 0; i < n; ++i){
        c[i] = a[i] - b[i];
    }

    if (acc_get_default_async() != q){
        err++;
    }

    #pragma acc update self(c[0:n]) async(q)
    while (!acc_async_test(q)) { }

    for (int i = 0; i < n; ++i) {
        if (fabs(c[i] - (a[i] - b[i])) > PRECISION){
            err++;
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n], c[0:n])

    free(a); 
    free(b); 
    free(c);
    return err;
}
#endif

#ifndef T3
int test3() {
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

#ifndef T4
int test4() {
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
        #pragma acc parallel loop async(acc_async_noval)
        for (int i = 0; i < n; ++i){
            c[i] = a[i] - b[i];
        }

        #pragma acc wait
    }

    for (int i = 0; i < n; ++i) {
        if (fabs(c[i] - (a[i] - b[i])) > PRECISION){
            err++;
        }
    }

    free(a); 
    free(b); 
    free(c);
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
#ifndef T3
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test3();
    }
    if (failed){
        failcode |= (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test4();
    }
    if (failed){
        failcode |= (1 << 3);
    }
#endif
    return failcode;
}
