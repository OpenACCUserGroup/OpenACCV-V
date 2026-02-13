// acc_pqr_list.c
//
// Feature under test (OpenACC 3.4, Section 1.6, Feb 2026):
// - A pqr-list must contain at least one item.
// - A pqr-list must not have a trailing comma.
//
// This test exercises valid pqr-list forms in:
//   - var-lists (copyin, copyout, present)
//   - int-expr-lists (wait)
//
// Only spec-compliant (non-empty, no trailing comma) forms are used.


#include "acc_testsuite.h"

#ifndef T1
//T1:syntax,pqr-list,runtime,construct-independent,V:3.4-
// int-expr-list non-empty (single item) via wait(1)
int test1(void){
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c){
        free(a); free(b); free(c);
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n]) async(1)
        for (int i = 0; i < n; ++i){
            c[i] = a[i] + b[i];
        }

        // Valid int-expr-list (non-empty)
        #pragma acc wait(1)
    }

    for (int i = 0; i < n; ++i){
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION) err++;
    }

    free(a); free(b); free(c);
    return err;
}
#endif

#ifndef T2
//T2:syntax,pqr-list,runtime,construct-independent,V:3.4-
// int-expr-list no trailing comma (multi-item list) via wait(1,2)
int test2(void){
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c){
        free(a); free(b); free(c);
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n]) async(1)
        for (int i = 0; i < n; ++i){
            c[i] = a[i] + b[i];
        }

        // second async region to make queue 2 meaningful
        #pragma acc parallel loop present(c[0:n]) async(2)
        for (int i = 0; i < n; ++i){
            c[i] = c[i];
        }

        // Valid multi-item int-expr-list with NO trailing comma
        #pragma acc wait(1,2)
    }

    for (int i = 0; i < n; ++i){
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION) err++;
    }

    free(a); free(b); free(c);
    return err;
}
#endif

#ifndef T3
//T3:syntax,pqr-list,runtime,construct-independent,V:3.4-
// var-list non-empty (single item) via copyin(a[0:n])
int test3(void){
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !c){
        free(a); free(c);
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    // Valid single-item var-list (non-empty)
    #pragma acc data copyin(a[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], c[0:n])
        for (int i = 0; i < n; ++i){
            c[i] = a[i] * 2;
        }
    }

    for (int i = 0; i < n; ++i){
        if (fabs(c[i] - (a[i] * 2)) > PRECISION) err++;
    }

    free(a); free(c);
    return err;
}
#endif

#ifndef T4
//T4:syntax,pqr-list,runtime,construct-independent,V:3.4-
// var-list no trailing comma (multi-item) via copyin(a[0:n], b[0:n]) and present(a,b,c)
int test4(void){
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c){
        free(a); free(b); free(c);
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    // Valid multi-item var-list with NO trailing comma
    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n])
        for (int i = 0; i < n; ++i){
            c[i] = a[i] + b[i];
        }
    }

    for (int i = 0; i < n; ++i){
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION) err++;
    }

    free(a); free(b); free(c);
    return err;
}
#endif

int main(void){
    int failcode = 0;
    int failed;

#ifndef T1
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i) failed += test1();
    if (failed) failcode |= (1 << 0);
#endif
#ifndef T2
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i) failed += test2();
    if (failed) failcode |= (1 << 1);
#endif
#ifndef T3
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i) failed += test3();
    if (failed) failcode |= (1 << 2);
#endif
#ifndef T4
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i) failed += test4();
    if (failed) failcode |= (1 << 3);
#endif

    return failcode;
}
