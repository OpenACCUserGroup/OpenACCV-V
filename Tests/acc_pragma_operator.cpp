// acc_pragma_operator.cpp
//
// Feature under test (OpenACC 3.4, Section 2.1, February 2026):
// - In C++, OpenACC directives may be expressed using either
//   #pragma acc ... or the equivalent _Pragma("acc ...") operator form.
//
// Notes:
// - T1: Uses _Pragma for data + parallel + loop and checks computation.
// - T2: Uses _Pragma for enter data / exit data and checks computation.
//


#include "acc_testsuite.h"
#include <openacc.h>
#include <cstdlib>
#include <cmath>

#ifndef T1
//T1:runtime,syntax,pragma,construct-independent,V:3.4-
// Use _Pragma form for data/parallel/loop and verify computation
int test1(){
    int err = 0;
    srand(SEED);

    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0.0;
    }

    _Pragma("acc data copyin(a[0:n], b[0:n]) copy(c[0:n])")
    {
        _Pragma("acc parallel")
        {
            _Pragma("acc loop")
            for (int i = 0; i < n; ++i){
                c[i] = a[i] + b[i];
            }
        }
    }

    for (int i = 0; i < n; ++i){
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION){
            err += 1;
        }
    }

    delete [] a;
    delete [] b;
    delete [] c;
    return err;
}
#endif

#ifndef T2
//T2:runtime,syntax,pragma,construct-independent,V:3.4-
// Use _Pragma form for enter/exit data directives
int test2(){
    int err = 0;
    srand(SEED);

    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];

    for (int i = 0; i < n; ++i){
        a[i] = rand() / (real_t)(RAND_MAX / 10);
        b[i] = rand() / (real_t)(RAND_MAX / 10);
        c[i] = 0.0;
    }

    _Pragma("acc enter data copyin(a[0:n], b[0:n]) create(c[0:n])")

    _Pragma("acc parallel present(a[0:n], b[0:n], c[0:n])")
    {
        _Pragma("acc loop")
        for (int i = 0; i < n; ++i){
            c[i] = (real_t)2.0 * a[i] - b[i];
        }
    }

    _Pragma("acc exit data copyout(c[0:n]) delete(a[0:n], b[0:n])")

    for (int i = 0; i < n; ++i){
        if (fabs(c[i] - ((real_t)2.0 * a[i] - b[i])) > PRECISION){
            err += 1;
        }
    }

    delete [] a;
    delete [] b;
    delete [] c;
    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed;

#ifndef T1
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test1();
    }
    if (failed != 0){
        failcode |= (1 << 0);
    }
#endif

#ifndef T2
    failed = 0;
    for (int i = 0; i < NUM_TEST_CALLS; ++i){
        failed += test2();
    }
    if (failed != 0){
        failcode |= (1 << 1);
    }
#endif

    return failcode;
}
