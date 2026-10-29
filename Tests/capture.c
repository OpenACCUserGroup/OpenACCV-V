// capture.c
//
// Feature under test (OpenACC 3.4, Sections 2.7.4, 2.7.9, and 2.7.10, April 2026):
// The capture modifier was added to data clauses to specify that a variable
// requires a discrete device-accessible copy, even when the implementation
// might otherwise use shared memory between the host and device.
//
// Tests:
// T1 – copy(capture:...): Verifies that a captured copy is created at the
//      start of a data region. The host modifies the variable after entry,
//      and the device computation must use the original captured values.
// T2 – copyout(capture:...): Verifies that a captured device copy is used
//      during execution and that results are correctly copied back to the
//      host at region exit. Host-side modifications after capture must not
//      affect device computation.
// T3 – create(capture:...): Verifies that a captured device-only copy is
//      created and used across compute regions. The host version of the
//      variable is modified after capture, and the device must use its own
//      independent copy while the host value remains unchanged.

#include "acc_testsuite.h"
#ifndef T1
//T1:data,structured-data,construct-independent,capture-modifier,V:3.4
int test1() {
    int err = 0;
    real_t *x = (real_t *)malloc(n * sizeof(real_t));
    for (int i = 0; i < n; ++i) {
        x[i] = 2;
    }
    #pragma acc data copy(capture:x[0:n])
    {
        for (int i = 0; i < n; ++i) {
            x[i] = 1;
        }

        #pragma acc parallel loop copy(x[0:n])
        for (int i = 0; i < n; ++i) {
            x[i] = x[i] + 1;
        }
    }

    for (int i = 0; i < n; ++i) {
        if (fabs(x[i] - 3) > PRECISION) {
            err += 1;
        }
    }
    free(x);
    return err;
}
#endif

#ifndef T2
//T2:data,structured-data,construct-independent,capture-modifier,V:3.4
int test2() {
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    for (int i = 0; i < n; ++i) {
        a[i] = -1;
    }
    #pragma acc data copyout(capture:a[0:n])
    {
        #pragma acc parallel loop present(a[0:n])
        for (int i = 0; i < n; ++i) {
            a[i] = 0;
        }
        for (int i = 0; i < n; ++i) {
            a[i] = 5;
        }
        #pragma acc parallel loop present(a[0:n])
        for (int i = 0; i < n; ++i) {
            a[i] = a[i] + 1;
        }
    }
    for (int i = 0; i < n; ++i) {
        if (fabs(a[i] - 1) > PRECISION) {
            err += 1;
        }
    }
    free(a);
    return err;
}
#endif
#ifndef T3
//T3:data,structured-data,construct-independent,capture-modifier,V:3.4
int test3() {
    int err = 0;
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *a_ref = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    srand(SEED);
    for (int i = 0; i < n; ++i) {
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        a_ref[i] = a[i];
        b[i] = 0;
    }
    #pragma acc data copy(a[0:n]) create(capture:b[0:n])
    {
        for (int i = 0; i < n; ++i) {
            b[i] = 5;
        }
        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int i = 0; i < n; ++i) {
            a[i] = a[i] + b[i];
        }
    }
    for (int i = 0; i < n; ++i) {
        if (fabs(a[i] - a_ref[i]) > PRECISION) {
            err += 1;
        }
        if (fabs(b[i] - 5) > PRECISION) {
            err += 1;
        }
    }
    free(a);
    free(a_ref);
    free(b);
    return err;
}
#endif
int main() {
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed = failed + test1();
    }
    if (failed != 0) {
        failcode = failcode + (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed = failed + test2();
    }
    if (failed != 0) {
        failcode = failcode + (1 << 1);
    }
#endif
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed = failed + test3();
    }
    if (failed != 0) {
        failcode = failcode + (1 << 2);
    }
#endif
    return failcode;
}
