#include "acc_testsuite.h"

#ifndef T1
//T1:data,structured-data,construct-independent,capture-modifier,V:3.4
int test1() {
    int err = 0;
    real_t *x = (real_t *)malloc(n * sizeof(real_t));

    for (int i = 0; i < n; ++i) {
        x[i] = 0;
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
        if (fabs(x[i] - 1) > PRECISION) {
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
        #pragma acc parallel loop copyout(a[0:n])
        for (int i = 0; i < n; ++i) {
            a[i] = (real_t)(i + 1);
        }
    }

    for (int i = 0; i < n; ++i) {
        if (fabs(a[i] - (real_t)(i + 1)) > PRECISION) {
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
        b[i] = -1;
    }

    #pragma acc data copy(a[0:n]) create(capture:b[0:n])
    {
        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int i = 0; i < n; ++i) {
            b[i] = 1;
        }

        #pragma acc parallel loop present(a[0:n], b[0:n])
        for (int i = 0; i < n; ++i) {
            a[i] = a[i] + b[i];
        }
    }

    for (int i = 0; i < n; ++i) {
        if (fabs(a[i] - (a_ref[i] + 1)) > PRECISION) {
            err += 1;
        }
        if (fabs(b[i] - (-1)) > PRECISION) {
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
