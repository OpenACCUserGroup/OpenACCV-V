// acc_integral_constant_expression.c
#include "acc_testsuite.h"
#include <openacc.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>

// C integer constant expressions: enums + macros
enum { ICE_ENUM2 = 2, ICE_ENUM4 = 4, ICE_ENUM_LOWER = 3 };
#define ICE_MACRO2   (1 + 1)
#define ICE_TILE1    (2)
#define ICE_TILE2    (ICE_MACRO2)   // still ICE
#define ICE_LEN4     (4)

static int check_vec_add(const real_t* a, const real_t* b, const real_t* c, int nloc){
    int err = 0;
    for(int i=0;i<nloc;i++){
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION) err++;
    }
    return err;
}

#ifndef T1
//T1:syntax,collapse-clause,runtime,loop,V:3.4-
// collapse(2) literal ICE (positive, non-zero)
int test1(void){
    int err = 0;
    const int M = 64, N = 16;
    const int MN = M*N;
    real_t *a=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *b=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)MN*sizeof(real_t));
    if(!a||!b||!c){ free(a); free(b); free(c); return 1; }

    for(int i=0;i<MN;i++){ a[i]=(real_t)i; b[i]=(real_t)(2*i); c[i]=0; }

    #pragma acc data copyin(a[0:MN],b[0:MN]) copyout(c[0:MN])
    {
        #pragma acc parallel loop collapse(2)
        for(int i=0;i<M;i++){
            for(int j=0;j<N;j++){
                int idx = i*N + j;
                c[idx] = a[idx] + b[idx];
            }
        }
    }

    err += check_vec_add(a,b,c,MN);
    free(a); free(b); free(c);
    return err;
}
#endif

#ifndef T2
//T2:syntax,collapse-clause,runtime,loop,V:3.4-
// collapse(ICE_MACRO2) macro arithmetic ICE
int test2(void){
    int err = 0;
    const int M = 48, N = 12;
    const int MN = M*N;
    real_t *a=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *b=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)MN*sizeof(real_t));
    if(!a||!b||!c){ free(a); free(b); free(c); return 1; }

    for(int i=0;i<MN;i++){ a[i]=(real_t)(i+2); b[i]=(real_t)(i-1); c[i]=0; }

    #pragma acc data copyin(a[0:MN],b[0:MN]) copyout(c[0:MN])
    {
        #pragma acc parallel loop collapse(ICE_MACRO2)
        for(int i=0;i<M;i++){
            for(int j=0;j<N;j++){
                int idx = i*N + j;
                c[idx] = a[idx] + b[idx];
            }
        }
    }

    err += check_vec_add(a,b,c,MN);
    free(a); free(b); free(c);
    return err;
}
#endif

#ifndef T3
//T3:syntax,tile-clause,runtime,loop,V:3.4-
// tile(2) literal ICE
int test3(void){
    int err = 0;
    const int M = 256;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){ free(a); free(c); return 1; }

    for(int i=0;i<M;i++){ a[i]=(real_t)i; c[i]=0; }

    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop tile(ICE_TILE1)
        for(int i=0;i<M;i++){
            c[i] = a[i] * (real_t)2.0;
        }
    }

    for(int i=0;i<M;i++){
        if (fabs(c[i] - a[i]*(real_t)2.0) > PRECISION) err++;
    }
    free(a); free(c);
    return err;
}
#endif

#ifndef T4
//T4:syntax,tile-clause,runtime,loop,V:3.4-
// tile(ICE_ENUM2, ICE_MACRO2) enum + macro ICE
int test4(void){
    int err = 0;
    const int M = 64, N = 40;
    const int MN = M*N;
    real_t *a=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *b=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)MN*sizeof(real_t));
    if(!a||!b||!c){ free(a); free(b); free(c); return 1; }

    for(int i=0;i<MN;i++){ a[i]=(real_t)(i+1); b[i]=(real_t)(3*i); c[i]=0; }

    #pragma acc data copyin(a[0:MN],b[0:MN]) copyout(c[0:MN])
    {
        #pragma acc parallel loop tile(ICE_ENUM2, ICE_MACRO2)
        for(int i=0;i<M;i++){
            for(int j=0;j<N;j++){
                int idx = i*N + j;
                c[idx] = a[idx] + b[idx];
            }
        }
    }

    err += check_vec_add(a,b,c,MN);
    free(a); free(b); free(c);
    return err;
}
#endif

#ifndef T5
//T5:syntax,cache-directive,runtime,loop,V:3.4-
// cache: element index uses ICE (enum)
int test5(void){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){ free(a); free(c); return 1; }

    for(int i=0;i<M;i++){ a[i]=(real_t)i; c[i]=0; }

    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop
        for(int i=0;i<M;i++){
            #pragma acc cache(a[ICE_ENUM2])
            c[i] = a[i] + (real_t)1.0;
        }
    }

    for(int i=0;i<M;i++){
        if (fabs(c[i] - (a[i]+(real_t)1.0)) > PRECISION) err++;
    }
    free(a); free(c);
    return err;
}
#endif

#ifndef T6
//T6:syntax,cache-directive,runtime,loop,V:3.4-
// cache: subarray lower:length uses ICE lower + ICE length
int test6(void){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){ free(a); free(c); return 1; }

    for(int i=0;i<M;i++){ a[i]=(real_t)i; c[i]=a[i]; }

    #pragma acc data copyin(a[0:M]) copy(c[0:M])
    {
        #pragma acc parallel loop
        for(int i=0;i<M;i++){
            #pragma acc cache(a[ICE_ENUM_LOWER:ICE_LEN4])
            c[i] = c[i] + (real_t)2.0;
        }
    }

    for(int i=0;i<M;i++){
        if (fabs(c[i] - (a[i]+(real_t)2.0)) > PRECISION) err++;
    }
    free(a); free(c);
    return err;
}
#endif

#ifndef T7
//T7:syntax,gang-clause,runtime,loop,V:3.4-
// gang(dim:ICE_ENUM2) where dim is an integral-constant-expression (enum), must evaluate to 1..3
// NOTE: Some compilers may not support the 'dim:' keyword form yet; keep this as spec conformance coverage.
int test7(void){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){ free(a); free(c); return 1; }

    for(int i=0;i<M;i++){ a[i]=(real_t)i; c[i]=0; }

    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop gang(dim:ICE_ENUM2)
        for(int i=0;i<M;i++){
            c[i] = a[i] * (real_t)2.0;
        }
    }

    for(int i=0;i<M;i++){
        if (fabs(c[i] - a[i]*(real_t)2.0) > PRECISION) err++;
    }

    free(a); free(c);
    return err;
}
#endif

int main(void){
    int failcode=0, failed;
#ifndef T1
    failed=0; for(int i=0;i<NUM_TEST_CALLS;i++) failed+=test1(); if(failed) failcode|=(1<<0);
#endif
#ifndef T2
    failed=0; for(int i=0;i<NUM_TEST_CALLS;i++) failed+=test2(); if(failed) failcode|=(1<<1);
#endif
#ifndef T3
    failed=0; for(int i=0;i<NUM_TEST_CALLS;i++) failed+=test3(); if(failed) failcode|=(1<<2);
#endif
#ifndef T4
    failed=0; for(int i=0;i<NUM_TEST_CALLS;i++) failed+=test4(); if(failed) failcode|=(1<<3);
#endif
#ifndef T5
    failed=0; for(int i=0;i<NUM_TEST_CALLS;i++) failed+=test5(); if(failed) failcode|=(1<<4);
#endif
#ifndef T6
    failed=0; for(int i=0;i<NUM_TEST_CALLS;i++) failed+=test6(); if(failed) failcode|=(1<<5);
#endif
#ifndef T7
    failed=0; for(int i=0;i<NUM_TEST_CALLS;i++) failed+=test7(); if(failed) failcode|=(1<<6);
#endif
    return failcode;
}
