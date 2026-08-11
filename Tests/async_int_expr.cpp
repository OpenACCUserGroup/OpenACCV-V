// async_int_expr.cpp
//
// Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
// Compute constructs now use the async-argument and wait-argument
// grammar consistently with the rest of the specification.
//
// Test:
// T1 – Basic async expression and wait.

#include "acc_testsuite.h"
#include <openacc.h>
#include <cmath>
#include <cstdlib>

#ifndef T1
//T1:async,wait,runtime,compute-constructs,baseline,V:3.4-
int test1() {
    int err = 0;
    std::srand(SEED);

    real_t *a = (real_t*)std::malloc(n * sizeof(real_t));
    real_t *b = (real_t*)std::malloc(n * sizeof(real_t));
    real_t *c = (real_t*)std::malloc(n * sizeof(real_t));
    if (!a || !b || !c){
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        a[i] = std::rand() / (real_t)(RAND_MAX / 10);
        b[i] = std::rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    int q = 4;
    int q0 = q - q;

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n]) async(q0)
        for (int i = 0; i < n; ++i){
            c[i] = a[i] + b[i];
        }

        #pragma acc wait
    }

    for (int i = 0; i < n; ++i) {
        if (std::fabs(c[i] - (a[i] + b[i])) > PRECISION){
            err++;
        }
    }

    std::free(a);
    std::free(b);
    std::free(c);
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

    return failcode;
}
