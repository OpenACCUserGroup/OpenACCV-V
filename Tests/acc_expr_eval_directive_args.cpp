// acc_expr_eval_directive_args.cpp
// Feature under test (OpenACC 3.4, Section 2.1, Feb 2026):
// - Clarified user-visible behavior of evaluation of expressions in directive arguments.
//   A program must not depend on the order/number of evaluations of expressions in
//   clause/construct/directive arguments, nor on any side effects of those evaluations.
//
// Notes:
// - T1: uses complex but side-effect-free expressions in directive arguments; checks correctness.
// - T2: uses update ... if_present where data is not present; must be a no-op and not crash.
//       The section length expression may be elided; we do NOT assert call count.
// - T3: uses side-effecting expressions in directive arguments; we do NOT rely on side effects;
//       checks correctness only.

#include "acc_testsuite.h"
#include <openacc.h>
#include <cstdlib>
#include <cmath>

static int foo_pure(int x){
    return (x % 64) + 1;
}

static int bar_pure(int x){
    return (x % 32) + 1;
}

static volatile int size_calls = 0;

static int size_maybe_elided(int nval){
    size_calls = size_calls + 1;
    return nval;
}

#ifndef T1
//T1:syntax,expressions,runtime,construct-independent,V:3.4-
int test1(){
    int err = 0;
    int cond = 0;

    std::srand(SEED);

    real_t *a = (real_t *)std::malloc(n * sizeof(real_t));
    real_t *b = (real_t *)std::malloc(n * sizeof(real_t));
    real_t *c = (real_t *)std::malloc(n * sizeof(real_t));

    if (a == NULL || b == NULL || c == NULL){
        if (a != NULL){
            std::free(a);
        }
        if (b != NULL){
            std::free(b);
        }
        if (c != NULL){
            std::free(c);
        }
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = std::rand() / (real_t)(RAND_MAX / 10);
        b[i] = std::rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0;
    }

    if (n > 0){
        cond = 1;
    }
    else{
        cond = 0;
    }

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        #pragma acc parallel \
            if((cond == 1) && ((n / 2) > 0)) \
            num_gangs(foo_pure(n + 7)) \
            vector_length(((n % 128) + 1))
        {
            #pragma acc loop
            for (int i = 0; i < n; ++i){
                c[i] = a[i] + b[i];
            }
        }
    }

    for (int i = 0; i < n; ++i){
        if (std::fabs(c[i] - (a[i] + b[i])) > PRECISION){
            err = err + 1;
        }
    }

    std::free(a);
    std::free(b);
    std::free(c);

    return err;
}
#endif

#ifndef T2
//T2:syntax,expressions,runtime,construct-independent,V:3.4-
int test2(){
    int err = 0;

    real_t *a = (real_t *)std::malloc(n * sizeof(real_t));
    if (a == NULL){
        return 1;
    }

    for (int i = 0; i < n; ++i){
        a[i] = (real_t)i;
    }

    // Ensure 'a' is NOT present on device: do NOT enter/create any data.
    size_calls = 0;
    #pragma acc update device(a[0:size_maybe_elided(n)]) if_present

    // User-visible behavior: no crash; host values unchanged.
    for (int i = 0; i < n; ++i){
        if (std::fabs(a[i] - (real_t)i) > PRECISION){
            err = err + 1;
        }
    }

    // DO NOT assert anything about size_calls (may be 0, 1, or more).

    std::free(a);
    return err;
}
#endif

#ifndef T3
//T3:syntax,expressions,runtime,construct-independent,V:3.4-
int test3(){
    int err = 0;

    std::srand(SEED);

    real_t *a = (real_t *)std::malloc(n * sizeof(real_t));
    real_t *b = (real_t *)std::malloc(n * sizeof(real_t));
    real_t *c = (real_t *)std::malloc(n * sizeof(real_t));

    if (a == NULL || b == NULL || c == NULL){
        if (a != NULL){
            std::free(a);
        }
        if (b != NULL){
            std::free(b);
        }
        if (c != NULL){
            std::free(c);
        }
        return 1;
    }

    for (int k = 0; k < n; ++k){
        a[k] = std::rand() / (real_t)(RAND_MAX / 10);
        b[k] = std::rand() / (real_t)(RAND_MAX / 10);
        c[k] = 0;
    }

    // Side-effecting expressions in directive arguments.
    // Per Section 2.1, evaluation order/number is unspecified.
    // We do NOT use i afterward (do not rely on side effects).
    int i = 0;

    #pragma acc data copyin(a[0:n], b[0:n]) copy(c[0:n])
    {
        #pragma acc parallel \
            num_gangs(foo_pure(++i)) \
            num_workers(bar_pure(i++)) \
            async(((i + 1) % 3) + 1)
        {
            #pragma acc loop
            for (int k = 0; k < n; ++k){
                c[k] = a[k] + b[k];
            }
        }

        #pragma acc wait
    }

    for (int k = 0; k < n; ++k){
        if (std::fabs(c[k] - (a[k] + b[k])) > PRECISION){
            err = err + 1;
        }
    }

    // DO NOT assert anything about i.

    std::free(a);
    std::free(b);
    std::free(c);

    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed = 0;

#ifndef T1
    failed = 0;
    for (int t = 0; t < NUM_TEST_CALLS; ++t){
        failed = failed + test1();
    }
    if (failed != 0){
        failcode = failcode + (1 << 0);
    }
#endif

#ifndef T2
    failed = 0;
    for (int t = 0; t < NUM_TEST_CALLS; ++t){
        failed = failed + test2();
    }
    if (failed != 0){
        failcode = failcode + (1 << 1);
    }
#endif

#ifndef T3
    failed = 0;
    for (int t = 0; t < NUM_TEST_CALLS; ++t){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif

    return failcode;
}
