// copy_host_if.c
//
//Feature under test (OpenACC 3.4, Sections 2.6.5, 2.6.6, and 2.8. March 2026):
// The specification clarifies that the data,
// and host_data constructs may contain at most one if clause.
// These tests verify correct behavior when a single valid if clause
// is used on these constructs.
//
// Tests:
// T1 - combined constructs with single if clauses:
//      Uses both data and host_data constructs within the same region,
//      each containing a single if clause, verifying that multiple
//      constructs can independently use valid if clauses.


#include "acc_testsuite.h"
#include <openacc.h>

#ifndef T1
//T1:syntax,data,host-data,if-clause,construct-independent,V:3.4-
int test1(){
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    int host = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0.0;
    }

    #pragma acc data copy(a[0:n], b[0:n]) if(1)
    {
        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int x = 0; x < n; ++x){
            b[x] = a[x] * 2.0;
        }

        #pragma acc host_data use_device(b) if(host)
        {
            ;
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - (a[x] * 2.0)) > PRECISION){
            err += 1;
        }
    }

    free(a);
    free(b);

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
