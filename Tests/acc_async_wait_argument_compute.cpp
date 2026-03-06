// acc_async_wait_argument_compute.cpp
//
// Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
// Compute constructs now use the async-argument and wait-argument
// grammar consistently with the rest of the specification.
//
// Tests:
// T1 – Basic async expression and wait.
// T2 – wait(queues: ...) syntax.
// T3 – Default async queue behavior using runtime API.
// T4 – wait(devnum: ...) syntax.
// T5 – wait(devnum: ... : queues: ...) syntax.
// T6 – async(acc_async_sync).
// T7 – async(acc_async_noval).

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

#ifndef T2
int test2() {
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

#ifndef T3
int test3() {
    int err = 0;
    std::srand(SEED);

    const int q = 7;
    acc_set_default_async(q);

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

    #pragma acc enter data copyin(a[0:n], b[0:n]) create(c[0:n])

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
        if (std::fabs(c[i] - (a[i] - b[i])) > PRECISION){
            err++;
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n], c[0:n])

    std::free(a);
    std::free(b); 
    std::free(c);
    return err;
}
#endif

#ifndef T4
int test4() {
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

#ifndef T5
int test5() {
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

#ifndef T6
int test6() {
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
        #pragma acc parallel loop async(acc_async_sync)
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

#ifndef T7
int test7() {
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
        #pragma acc parallel loop async(acc_async_noval)
        for (int i = 0; i < n; ++i){
            c[i] = a[i] - b[i];
        }

        #pragma acc wait
    }

    for (int i = 0; i < n; ++i) {
        if (std::fabs(c[i] - (a[i] - b[i])) > PRECISION){
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
#ifndef T4
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test4();
    }
    if (failed){
        failcode |= (1 << 3);
    }
#endif
#ifndef T5
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test5();
    }
    if (failed){
        failcode |= (1 << 4);
    }
#endif
#ifndef T6
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test6();
    }
    if (failed){
        failcode |= (1 << 5);
    }
#endif
#ifndef T7
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test7();
    }
    if (failed){
        failcode |= (1 << 6);
    }
#endif

    return failcode;
}
