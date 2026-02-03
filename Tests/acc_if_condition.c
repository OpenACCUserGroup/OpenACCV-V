// acc_if_condition.c
// Validates OpenACC 3.4’s clarified definition of “condition” when used as an argument to the if clause,
// and verifies that if(condition) correctly gates directive execution (OpenACC 3.4, Section 1.6).
//
// Test strategy:
// 1) Behavior-verifiable data-directive gating (T1–T4):
//    Uses enter data / exit data with if(true/false) and checks device presence via acc_is_present.
//    This directly proves that the runtime treats if(false) as a no-op and if(true) as executing the directive.
//
// 2) Condition-form coverage for compute constructs (T5–T7):
//    Confirms the compiler accepts the full set of C-valid “condition” forms in if(...):
//      - integer scalar expression (T5)
//      - floating-point scalar condition (T6)  (nonzero => true in C)
//      - pointer scalar condition (T7)         (non-NULL => true in C)
//    Each compute test performs a simple device computation and verifies results on the host.
//

#include "acc_testsuite.h"
#include <openacc.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>

#ifndef T1
//T1:syntax,if-clause,runtime,enter-data,V:3.4-
// enter data if(0) => must be NO-OP (not present)
int test1(void){
    int err = 0;
    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    if (!a) return 1;
    for (int i = 0; i < n; ++i) a[i] = (real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(0)

    if (acc_is_present(a, (size_t)n * sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(1)
    free(a);
    return err;
}
#endif

#ifndef T2
//T2:syntax,if-clause,runtime,enter-data,V:3.4-
// enter data if(1) => must happen (present)
int test2(void){
    int err = 0;
    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    if (!a) return 1;
    for (int i = 0; i < n; ++i) a[i] = (real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(1)

    if (!acc_is_present(a, (size_t)n * sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(1)
    free(a);
    return err;
}
#endif

#ifndef T3
//T3:syntax,if-clause,runtime,exit-data,V:3.4-
// exit data delete if(0) => must be NO-OP (still present)
int test3(void){
    int err = 0;
    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    if (!a) return 1;
    for (int i = 0; i < n; ++i) a[i] = (real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(1)
    if (!acc_is_present(a, (size_t)n * sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(0)
    if (!acc_is_present(a, (size_t)n * sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(1)
    free(a);
    return err;
}
#endif

#ifndef T4
//T4:syntax,if-clause,runtime,exit-data,V:3.4-
// exit data delete if(1) => must delete (not present)
int test4(void){
    int err = 0;
    real_t *a = (real_t*)malloc(n * sizeof(real_t));
    if (!a) return 1;
    for (int i = 0; i < n; ++i) a[i] = (real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(1)
    if (!acc_is_present(a, (size_t)n * sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(1)
    if (acc_is_present(a, (size_t)n * sizeof(real_t))) err++;

    free(a);
    return err;
}
#endif

// ---------- compute-side condition FORM coverage (C rules: any scalar) ----------

#ifndef T5
//T5:syntax,if-clause,runtime,compute,V:3.4-
// int scalar expression as condition
int test5(void){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t*)malloc(n*sizeof(real_t));
    real_t *b = (real_t*)malloc(n*sizeof(real_t));
    real_t *c = (real_t*)malloc(n*sizeof(real_t));
    if (!a || !b || !c){ free(a); free(b); free(c); return 1; }

    for (int i=0;i<n;++i){ a[i]=rand()/(real_t)(RAND_MAX/10); b[i]=rand()/(real_t)(RAND_MAX/10); c[i]=0; }

    int cond_int = (n > 0); // runtime scalar int condition

    #pragma acc data copyin(a[0:n],b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n],b[0:n],c[0:n]) if(cond_int)
        for (int i=0;i<n;++i) c[i]=a[i]+b[i];
    }

    for (int i=0;i<n;++i) if (fabs(c[i]-(a[i]+b[i]))>PRECISION) err++;
    free(a); free(b); free(c);
    return err;
}
#endif

#ifndef T6
//T6:syntax,if-clause,runtime,compute,V:3.4-
// floating-point scalar as condition (C: nonzero => true)
int test6(void){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t*)malloc(n*sizeof(real_t));
    real_t *c = (real_t*)malloc(n*sizeof(real_t));
    if (!a || !c){ free(a); free(c); return 1; }

    for (int i=0;i<n;++i){ a[i]=rand()/(real_t)(RAND_MAX/10); c[i]=0; }

    real_t cond_real = (real_t)1.0; // nonzero scalar => true in C

    #pragma acc data copyin(a[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n],c[0:n]) if(cond_real)
        for (int i=0;i<n;++i) c[i]=a[i]*(real_t)2.0;
    }

    for (int i=0;i<n;++i) if (fabs(c[i]-(a[i]*(real_t)2.0))>PRECISION) err++;
    free(a); free(c);
    return err;
}
#endif

#ifndef T7
//T7:syntax,if-clause,runtime,compute,V:3.4-
// pointer scalar as condition (C: non-NULL => true)
int test7(void){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t*)malloc(n*sizeof(real_t));
    real_t *c = (real_t*)malloc(n*sizeof(real_t));
    if (!a || !c){ free(a); free(c); return 1; }

    for (int i=0;i<n;++i){ a[i]=rand()/(real_t)(RAND_MAX/10); c[i]=0; }

    void* cond_ptr = (void*)a; // non-NULL pointer => true in C

    #pragma acc data copyin(a[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n],c[0:n]) if(cond_ptr)
        for (int i=0;i<n;++i) c[i]=a[i]+(real_t)1.0;
    }

    for (int i=0;i<n;++i) if (fabs(c[i]-(a[i]+(real_t)1.0))>PRECISION) err++;
    free(a); free(c);
    return err;
}
#endif

int main(void){
    int failcode = 0, failed;

#ifndef T1
    failed=0; for(int i=0;i<NUM_TEST_CALLS;++i) failed+=test1(); if(failed) failcode|=(1<<0);
#endif
#ifndef T2
    failed=0; for(int i=0;i<NUM_TEST_CALLS;++i) failed+=test2(); if(failed) failcode|=(1<<1);
#endif
#ifndef T3
    failed=0; for(int i=0;i<NUM_TEST_CALLS;++i) failed+=test3(); if(failed) failcode|=(1<<2);
#endif
#ifndef T4
    failed=0; for(int i=0;i<NUM_TEST_CALLS;++i) failed+=test4(); if(failed) failcode|=(1<<3);
#endif
#ifndef T5
    failed=0; for(int i=0;i<NUM_TEST_CALLS;++i) failed+=test5(); if(failed) failcode|=(1<<4);
#endif
#ifndef T6
    failed=0; for(int i=0;i<NUM_TEST_CALLS;++i) failed+=test6(); if(failed) failcode|=(1<<5);
#endif
#ifndef T7
    failed=0; for(int i=0;i<NUM_TEST_CALLS;++i) failed+=test7(); if(failed) failcode|=(1<<6);
#endif

    return failcode;
}
