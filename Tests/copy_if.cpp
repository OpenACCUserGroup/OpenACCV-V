// copy_if.cpp
//
//Feature under test (OpenACC 3.4, Sections 2.6.5, 2.6.6, and 2.8. March 2026):
// The specification clarifies that the data,
// and host_data constructs may contain at most one if clause.
// These tests verify correct behavior when a single valid if clause
// is used on these constructs.
//
// Tests:
// T1 – data construct with single if clause:
//      Uses a data region with copy and a single if(dev)
//      clause controlling device execution.
//


#include "acc_testsuite.h"
#include <openacc.h>

#ifndef T1
//T1:syntax,data,if-clause,construct-independent,V:3.4-
int test1(){
    int err = 0;
    srand(SEED);

    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    int dev = 1;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
    }

    #pragma acc data copy(a[0:n], b[0:n]) if(dev)
    {
        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int x = 0; x < n; ++x){
            b[x] = a[x] * 2.0;
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - (a[x] * 2.0)) > PRECISION){
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
        failed += test1();
    }
    if (failed != 0){
        failcode += (1 << 0);
    }
#endif

    return failcode;
}
