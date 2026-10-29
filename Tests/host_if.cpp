// host_if.cpp
//
// Feature under test (OpenACC 3.4, Sections 2.6.5, 2.6.6, and 2.8. March 2026):
// The specification clarifies that the data, enter data, exit data,
// and host_data constructs may contain at most one if clause.
// These tests verify correct behavior when a single valid if clause
// is used on these constructs.
//
// Tests:
// T1 - host_data construct with single if clause:
//      Uses host_data use_device(...) with if(dev) to confirm
//      that device pointer access functions correctly.
//
#include "acc_testsuite.h"
#include <openacc.h>

#ifndef T1
int test1(){
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
