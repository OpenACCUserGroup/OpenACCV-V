// data_copy_clauses_always_modifiers.c
//
// Feature under test (OpenACC 3.4, Sections 2.7.7, 2.7.8, 2.7.9, April 2026):
// Added the always, alwaysin, and alwaysout modifiers to the
// copy, copyin, and copyout data clauses. These modifiers force
// data movement between the host and device even when data is
// already present on the device or still has an active lifetime.
//
// Tests:
// T1 – copy(alwaysin:...): Verifies that when a variable is already
//      present on the device, the alwaysin modifier forces a fresh
//      transfer from the host to the device. The host copy is modified
//      after enter data, and the device computation must use the
//      updated host values.
//
// T2 – copy(alwaysout:...): Verifies that when a variable has an
//      existing device lifetime, the alwaysout modifier forces a
//      transfer from the device back to the host at the end of the
//      structured data region. A host-side check is performed
//      immediately after the region to confirm the update.
//
// T3 – copy(always:...): Verifies that the always modifier forces both
//      transfer-in and transfer-out. The host copy is modified after
//      enter data, the device performs computation, and the final host
//      result must reflect both the updated input and device computation.
//
// T4 – copyin(alwaysin:...): Verifies that alwaysin works with copyin
//      clauses. When a variable is already present on the device, the
//      modifier forces the updated host values to be used on the device.
//
// T5 – copyin(always:...): Verifies that always also forces transfer-in
//      when used with copyin. This test ensures that always behaves like
//      alwaysin for host-to-device movement.
//
// T6 – copyout(alwaysout:...): Verifies that alwaysout forces transfer-out
//      for variables computed on the device, even when they are already
//      present due to an outer data lifetime. A host-side check is performed
//      immediately after the data region to confirm the update occurred
//      before device data is deleted.
//
// T7 – copyout(always:...): Verifies that always also forces transfer-out
//      when used with copyout. This test confirms that always behaves like
//      alwaysout for device-to-host movement.

#include "acc_testsuite.h"

#ifndef T1
int test1() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *expected = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n])

    for (int x = 0; x < n; ++x) {
        a[x] = (real_t)(x + 1) * 3.0;
        expected[x] = a[x];
    }

    #pragma acc data copy(alwaysin:a[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], c[0:n])
        for (int x = 0; x < n; ++x) {
            c[x] = a[x];
        }
    }

    #pragma acc exit data delete(a[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - expected[x]) > PRECISION) {
            err += 1;
        }
    }

    free(a);
    free(c);
    free(expected);
    return err;
}
#endif

#ifndef T2
int test2() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *expected = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x) {
        a[x] = 0;
        expected[x] = (real_t)(x + 5);
    }

    #pragma acc enter data create(a[0:n])

    #pragma acc data copy(alwaysout:a[0:n])
    {
        #pragma acc parallel loop present(a[0:n])
        for (int x = 0; x < n; ++x) {
            a[x] = (real_t)(x + 5);
        }
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(a[x] - expected[x]) > PRECISION) {
            err += 1;
        }
    }

    #pragma acc exit data delete(a[0:n])

    free(a);
    free(expected);
    return err;
}
#endif

#ifndef T3
int test3() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *expected = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc enter data copyin(a[0:n])

    for (int x = 0; x < n; ++x) {
        a[x] = (real_t)(x + 2);
        expected[x] = 2 * a[x];
    }

    #pragma acc data copy(always:a[0:n])
    {
        #pragma acc parallel loop present(a[0:n])
        for (int x = 0; x < n; ++x) {
            a[x] = a[x] * 2;
        }
    }

    #pragma acc exit data delete(a[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(a[x] - expected[x]) > PRECISION) {
            err += 1;
        }
    }

    free(a);
    free(expected);
    return err;
}
#endif

#ifndef T4
int test4() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *expected = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n])

    for (int x = 0; x < n; ++x) {
        a[x] = (real_t)(10 + x);
        expected[x] = a[x];
    }

    #pragma acc data copyin(alwaysin:a[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], c[0:n])
        for (int x = 0; x < n; ++x) {
            c[x] = a[x];
        }
    }

    #pragma acc exit data delete(a[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - expected[x]) > PRECISION) {
            err += 1;
        }
    }

    free(a);
    free(c);
    free(expected);
    return err;
}
#endif

#ifndef T5
int test5() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    real_t *expected = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n])

    for (int x = 0; x < n; ++x) {
        a[x] = (real_t)(100 + 2 * x);
        expected[x] = a[x];
    }

    #pragma acc data copyin(always:a[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n], c[0:n])
        for (int x = 0; x < n; ++x) {
            c[x] = a[x];
        }
    }

    #pragma acc exit data delete(a[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - expected[x]) > PRECISION) {
            err += 1;
        }
    }

    free(a);
    free(c);
    free(expected);
    return err;
}
#endif

#ifndef T6
int test6() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *expected = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
        expected[x] = 3 * a[x];
    }

    #pragma acc enter data copyin(a[0:n]) create(b[0:n])

    #pragma acc data present(a[0:n]) copyout(alwaysout:b[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int x = 0; x < n; ++x) {
            b[x] = 3 * a[x];
        }
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(b[x] - expected[x]) > PRECISION) {
            err += 1;
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    free(a);
    free(b);
    free(expected);
    return err;
}
#endif

#ifndef T7
int test7() {
    int err = 0;
    srand(SEED);

    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *expected = (real_t *)malloc(n * sizeof(real_t));
 
    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
        expected[x] = a[x] + 7;
    }

    #pragma acc enter data copyin(a[0:n]) create(b[0:n])

    #pragma acc data present(a[0:n]) copyout(always:b[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int x = 0; x < n; ++x) {
            b[x] = a[x] + 7;
        }
    }

    for (int x = 0; x < n; ++x) {
        if (fabs(b[x] - expected[x]) > PRECISION) {
            err += 1;
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    free(a);
    free(b);
    free(expected);
    return err;
}
#endif

int main() {
    int failcode = 0;
    int failed;

#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed += test1();
    }
    if (failed != 0) {
        failcode += (1 << 0);
    }
#endif

#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed += test2();
    }
    if (failed != 0) {
        failcode += (1 << 1);
    }
#endif

#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed += test3();
    }
    if (failed != 0) {
        failcode += (1 << 2);
    }
#endif

#ifndef T4
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed += test4();
    }
    if (failed != 0) {
        failcode += (1 << 3);
    }
#endif

#ifndef T5
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed += test5();
    }
    if (failed != 0) {
        failcode += (1 << 4);
    }
#endif

#ifndef T6
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed += test6();
    }
    if (failed != 0) {
        failcode += (1 << 5);
    }
#endif

#ifndef T7
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed += test7();
    }
    if (failed != 0) {
        failcode += (1 << 6);
    }
#endif

    return failcode;
}
