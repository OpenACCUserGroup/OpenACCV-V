// acc_nested_procedure_boundaries.cpp
//
// Feature under test (OpenACC 3.4; clarified procedure-boundary analysis):
// - Clarified analysis of implicit data attributes and parallelism across
//   boundaries of procedures that can appear within other procedures
//   (e.g., C++ lambdas, C++ class member functions).
//
// Notes:
// - T1 (C++ lambda in parallel loop): Calls a lambda from within an
//   OpenACC 'parallel loop' region while relying on the surrounding data
//   region + default(present) and a captured scalar. Exercises implicit
//   data attribute analysis for variables referenced through a lambda.
// - T2 (C++ member function in parallel loop): Calls a class member
//   function from within an OpenACC 'parallel loop' region using a
//   firstprivate object containing pointer members and a scalar member.
//   Exercises procedure-boundary analysis for class/struct objects and
//   pointer members used on the device.
// - T3 (acc loop inside a lambda): Places an OpenACC 'loop' directive
//   inside a lambda body and invokes it within an OpenACC 'parallel'
//   region. Exercises loop/parallelism assoc

#include "acc_testsuite.h"

#include <cstdlib>
#include <cmath>

#ifndef T1
//T1:runtime,data,implicit-data,procedure-boundary,cxx-lambda,construct-independent,V:3.4-
int test1() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c) return 1;

    for (int i = 0; i < n; ++i) {
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    // Capture-by-value scalar should behave like firstprivate in device code.
    const real_t scale = (real_t)3.0;

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        // Lambda defined inside the region; called from within parallel loop.
        auto op = [=](int i) {
            // Uses a, b, c from the surrounding scope; compiler must analyze
            // those references correctly across the lambda boundary.
            c[i] = a[i] + b[i] + scale;
        };

        #pragma acc parallel loop default(present)
        for (int i = 0; i < n; ++i) {
            op(i);
        }
    }

    for (int i = 0; i < n; ++i) {
        real_t expect = a[i] + b[i] + scale;
        if (fabs(c[i] - expect) > PRECISION) err++;
    }

    free(a);
    free(b);
    free(c);
    return err;
}
#endif

#ifndef T2
//T2:runtime,data,implicit-data,procedure-boundary,cxx-member-function,construct-independent,V:3.4-
int test2() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c) return 1;

    for (int i = 0; i < n; ++i) {
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    struct Worker {
        real_t *a;
        real_t *b;
        real_t *c;
        real_t bias;   // scalar member should transfer with firstprivate-like behavior

        // Member function called on device; references through member pointers.
        void work(int i) const {
            c[i] = a[i] * bias + b[i];
        }
    };

    Worker w;
    w.a = a; w.b = b; w.c = c;
    w.bias = (real_t)2.0;

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        // w is used inside device region; it should be handled as firstprivate
        // and its pointer members should refer to present device data.
        #pragma acc parallel loop default(present) firstprivate(w)
        for (int i = 0; i < n; ++i) {
            w.work(i);
        }
    }

    for (int i = 0; i < n; ++i) {
        real_t expect = a[i] * w.bias + b[i];
        if (fabs(c[i] - expect) > PRECISION) err++;
    }

    free(a);
    free(b);
    free(c);
    return err;
}
#endif

#ifndef T3
//T3:runtime,data,parallelism,procedure-boundary,cxx-lambda-loop,construct-independent,V:3.4-
int test3() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    real_t *b = (real_t*)malloc(n * sizeof(real_t));
    real_t *c = (real_t*)malloc(n * sizeof(real_t));
    if (!a || !b || !c) return 1;

    for (int i = 0; i < n; ++i) {
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    const real_t alpha = (real_t)1.5;

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        // We place the OpenACC loop directive inside a lambda body.
        // This stresses that loop/parallel analysis remains valid across
        // the lambda boundary (i.e., directives in nested procedures).
        auto do_loop = [=]() {
            #pragma acc loop
            for (int i = 0; i < n; ++i) {
                c[i] = alpha * a[i] - b[i];
            }
        };

        #pragma acc parallel default(present)
        {
            do_loop();
        }
    }

    for (int i = 0; i < n; ++i) {
        real_t expect = alpha * a[i] - b[i];
        if (fabs(c[i] - expect) > PRECISION) err++;
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
    for (int i = 0; i < NUM_TEST_CALLS; ++i) failed += test1();
    if (failed != 0) failcode += (1 << 0);
#endif

#ifndef T2
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i) failed += test2();
    if (failed != 0) failcode += (1 << 1);
#endif

#ifndef T3
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i) failed += test3();
    if (failed != 0) failcode += (1 << 2);
#endif

    return failcode;
}
