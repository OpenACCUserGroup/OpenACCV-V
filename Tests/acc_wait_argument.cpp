// acc_wait_argument.cpp
//
// Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
// Compute constructs now use the async-argument and wait-argument
// grammar consistently with the rest of the specification.
//
// Tests:
// T1 – wait(queues: ...) syntax.
// T2 – wait(devnum: ...) syntax.
// T3 – wait(devnum: ... : queues: ...) syntax.

#include "acc_testsuite.h"
#include <openacc.h>
#include <cmath>
#include <cstdlib>

#ifndef T1
int test1() {
    int err = 0;
    std::srand(SEED);

    real_t *a = (real_t*)std::malloc(n * sizeof(real_t));
    real_t *b = (real_t*)std::malloc(n * sizeof(real_t));
    real_t *c = (real_t*)std::malloc(n * sizeof(real_t));
    real_t *d = (real_t*)std::malloc(n * sizeof(real_t));
    real_t *e = (real_t*)std::malloc(n * sizeof(real_t));
    if (!a || !b || !c || !d || !e){
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        a[i] = std::rand() / (real_t)(RAND_MAX / 10);
        b[i] = std::rand() / (real_t)(RAND_MAX / 10);
        c[i] = d[i] = e[i] = 0;
    }

    #pragma acc enter data copyin(a[0:n], b[0:n]) create(c[0:n], d[0:n], e[0:n])

    #pragma acc parallel loop present(a[0:n], c[0:n]) async(1)
    for (int i = 0; i < n; ++i){
        c[i] = a[i] * (real_t)2;
    }

    #pragma acc parallel loop present(b[0:n], d[0:n]) async(2)
    for (int i = 0; i < n; ++i){
        d[i] = b[i] * (real_t)3;
    }

    #pragma acc parallel loop present(c[0:n], d[0:n], e[0:n]) async(3) wait(queues: 1, 2)
    for (int i = 0; i < n; ++i){
        e[i] = c[i] + d[i];
    }

    #pragma acc update self(e[0:n]) async(3)
    while (!acc_async_test(3)) { }

    for (int i = 0; i < n; ++i) {
        if (std::fabs(e[i] - (a[i]*(real_t)2 + b[i]*(real_t)3)) > PRECISION){
            err++;
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n], c[0:n], d[0:n], e[0:n])

    std::free(a); 
    std::free(b); 
    std::free(c); 
    std::free(d); 
    std::free(e);
    return err;
}
#endif

#ifndef T2
int test2() {
    int err = 0;
    std::srand(SEED);

    real_t *a = (real_t*)std::malloc(n * sizeof(real_t));
    real_t *b = (real_t*)std::malloc(n * sizeof(real_t));
    if (!a || !b){
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = (real_t)1; 
        b[i] = (real_t)0; 
    }

    #pragma acc data copy(a[0:n], b[0:n])
    {
        #pragma acc parallel loop async(1)
        for (int i = 0; i < n; ++i){
            b[i] = a[i] * (real_t)2;
        }

        #pragma acc parallel loop async(2) wait(devnum: 0 : 1)
        for (int i = 0; i < n; ++i){
            a[i] = b[i] + (real_t)1;
        }

        #pragma acc update self(a[0:n]) async(2)
        while (!acc_async_test(2)) { }
    }

    for (int i = 0; i < n; ++i) {
        if (std::fabs(a[i] - (real_t)3) > PRECISION){
            err++;
        }
    }

    std::free(a); 
    std::free(b);
    return err;
}
#endif

#ifndef T3
int test3() {
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

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        #pragma acc parallel loop async(1)
        for (int i = 0; i < n; ++i){
            c[i] = a[i] + b[i];
        }

        #pragma acc parallel loop async(2) wait(devnum: 0 : queues: 1)
        for (int i = 0; i < n; ++i){
            c[i] = c[i] * (real_t)2;
        }

        #pragma acc update self(c[0:n]) async(2)
        while (!acc_async_test(2)) { }
    }

    for (int i = 0; i < n; ++i) {
        if (std::fabs(c[i] - (real_t)2*(a[i] + b[i])) > PRECISION){
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
    return failcode;
}
