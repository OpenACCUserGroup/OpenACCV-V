// acc_single_if_clause_data.cpp
//
// Feature under test (OpenACC 3.4, Sections 2.6.5, 2.6.6, 2.8. March 2026):
// The specification clarifies that the data, enter data, exit data,
// and host_data constructs may contain at most one if clause.
// These tests verify correct behavior when a single valid if clause
// is used on these constructs in a C++ environment.
//
// Tests:
// T1 – data construct with single if clause:
//      Uses a data region with copyin/copyout and a single if(dev)
//      clause controlling device execution.
//
// T2 – enter data / exit data with single if clause:
//      Uses enter data and exit data directives with if(dev) to
//      manage device memory and verify correct computation.
//
// T3 – host_data construct with single if clause:
//      Uses host_data use_device(...) with if(dev) to confirm
//      that device pointer access functions correctly.
//
// T4 – combined constructs with single if clauses:
//      Uses both data and host_data constructs within the same
//      region to verify that each construct may independently
//      contain a single if clause.

#include "acc_testsuite.h"
#include <openacc.h>

#ifndef T1
int test1(){
    int err = 0;
    srand(SEED);

    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    int dev = 1;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n]) if(dev)
    {
        #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n])
        for (int x = 0; x < n; ++x){
            c[x] = a[x] + b[x];
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    delete[] a;
    delete[] b;
    delete[] c;

    return err;
}
#endif

#ifndef T2
int test2(){
    int err = 0;
    srand(SEED);

    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    int dev = 1;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc enter data copyin(a[0:n], b[0:n]) create(c[0:n]) if(dev)

    #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n])
    for (int x = 0; x < n; ++x){
        c[x] = a[x] + b[x];
    }

    #pragma acc exit data copyout(c[0:n]) delete(a[0:n], b[0:n]) if(dev)

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    delete[] a;
    delete[] b;
    delete[] c;

    return err;
}
#endif

#ifndef T3
int test3(){
    int err = 0;
    srand(SEED);

    real_t *a = new real_t[n];
    real_t *seen_ptr = NULL;
    int dev = 1;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copyin(a[0:n])
    {
        #pragma acc host_data use_device(a) if(dev)
        {
            seen_ptr = a;
        }
    }

    if (seen_ptr == NULL){
        err += 1;
    }

    delete[] a;

    return err;
}
#endif

#ifndef T4
int test4(){
    int err = 0;
    srand(SEED);

    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    int host = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
    }

    #pragma acc data copyin(a[0:n]) copyout(b[0:n]) if(1)
    {
        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int x = 0; x < n; ++x){
            b[x] = a[x] * 2;
        }

        #pragma acc host_data use_device(b) if(host)
        {
            ;
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - (a[x] * 2)) > PRECISION){
            err += 1;
        }
    }

    delete[] a;
    delete[] b;

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
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test4();
    }
    if (failed != 0){
        failcode = failcode + (1 << 3);
    }
#endif
    return failcode;
}
