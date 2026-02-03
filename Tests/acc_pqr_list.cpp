// acc_pqr_list.cpp
// Validates correct usage of OpenACC pqr-lists in C++ as clarified in OpenACC 3.4 Section 1.6.
// Ensures that pqr-lists appearing in OpenACC clauses:
//   - are non-empty
//   - do not contain trailing commas
// The tests cover both var-lists (copyin, copyout, present) and int-expr-lists (wait),
// using single-item and multi-item lists in C++ compilation mode.
// Device execution results are compared against expected host values to confirm
// correct runtime behavior in addition to syntactic acceptance.

#include "acc_testsuite.h"
#include <cstdlib>
#include <cmath>

#ifndef T1
//T1:syntax,pqr-list,runtime,construct-independent,V:3.4-
int test1(){
    int err = 0;
    srand(SEED);

    real_t* a = (real_t*)malloc(n * sizeof(real_t));
    real_t* b = (real_t*)malloc(n * sizeof(real_t));
    real_t* c = (real_t*)malloc(n * sizeof(real_t));
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
int test2(){
    int err = 0;
    srand(SEED);

    real_t* a = (real_t*)malloc(n * sizeof(real_t));
    real_t* b = (real_t*)malloc(n * sizeof(real_t));
    real_t* c = (real_t*)malloc(n * sizeof(real_t));
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
        #pragma acc parallel loop present(c[0:n]) async(2)
        for (int i = 0; i < n; ++i){
            c[i] = c[i];
        }
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
int test3(){
    int err = 0;
    srand(SEED);

    real_t* a = (real_t*)malloc(n * sizeof(real_t));
    real_t* c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !c){
        free(a); free(c);
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

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
int test4(){
    int err = 0;
    srand(SEED);

    real_t* a = (real_t*)malloc(n * sizeof(real_t));
    real_t* b = (real_t*)malloc(n * sizeof(real_t));
    real_t* c = (real_t*)malloc(n * sizeof(real_t));
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

int main(){
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
