// acc_integral_constant_expression.c
//
//Feature under test (OpenACC 3.4, Section 1.6, Feb 2026):
// - Clause arguments that require an integral-constant-expression accept
//    C integral constant expressions (macros and enum values).
// 
//  Notes:
//  T1: collapse() accepts an integral-constant-expression (ICE).
//   This test uses a macro ICE in collapse() and checks runtime correctness.
//  
//  T2: tile() accepts an integral-constant-expression (ICE).
//   This test uses a macro ICE in tile() and checks runtime correctness.
//
//  T3: tile() accepts ICE values (enums/macros).
//   This test mixes enum + macro ICE in tile() and checks correctness.
//
//  T4: cache() indexing accepts an integral-constant-expression (ICE).
//   This test uses an enum ICE as the cache element index.
//
//  T5: cache() subarray slices accept ICE values.
//   This test uses enum/macro ICE for lower:length in cache().
//
//  T6: gang(dim:) accepts an integral-constant-expression (ICE) (valid range 1..3).
//   This test uses an enum ICE in gang(dim:) and checks runtime correctness.
// – Some compilers may not support the 'dim:' keyword form yet; keep this as spec conformance coverage.
//

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

#ifndef T1
int test1(void){
    int err = 0;
    const int M = 48, N = 12;
    const int MN = M*N;
    real_t *a=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *b=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)MN*sizeof(real_t));
    if(!a||!b||!c){
        free(a);
        free(b);
        free(c);
        return 1;
    }
    for(int i=0;i<MN;i++){ 
        a[i]=(real_t)(i+2); 
        b[i]=(real_t)(i-1); 
        c[i]=0; 
    }
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
    for (int i = 0; i < MN; ++i) {
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION){
            err++;
        }
    }

    free(a);
    free(b);
    free(c);
    return err;
}
#endif
#ifndef T2
int test2(void){
    int err = 0;
    const int M = 256;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        free(a);
        free(c);
        return 1;
    }
    for(int i=0;i<M;i++){
        a[i]=(real_t)i; 
        c[i]=0; 
    }
    
    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop tile(ICE_TILE1)
        for(int i=0;i<M;i++){
            c[i] = a[i] * (real_t)2.0;
        }
    }
    for(int i=0;i<M;i++){
        if (fabs(c[i] - a[i]*(real_t)2.0) > PRECISION){
            err++;
        }
    }
    free(a);
    free(c);
    return err;
}
#endif
#ifndef T3
int test3(void){
    int err = 0;
    const int M = 64, N = 40;
    const int MN = M*N;
    real_t *a=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *b=(real_t*)malloc((size_t)MN*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)MN*sizeof(real_t));
    if(!a||!b||!c){
        free(a);
        free(b);
        free(c);
        return 1;
    }
    
    for(int i=0;i<MN;i++){
        a[i]=(real_t)(i+1); 
        b[i]=(real_t)(3*i); 
        c[i]=0; 
    }
    
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
    for (int i = 0; i < MN; ++i) {
        if (fabs(c[i] - (a[i] + b[i])) > PRECISION){
            err++;
        }
    }

    free(a);
    free(b);
    free(c);
    return err;
}
#endif
#ifndef T4
int test4(void){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        free(a);
        free(c);
        return 1;
    }
    
    for(int i=0;i<M;i++){
        a[i]=(real_t)i;
        c[i]=0; 
    }
    
    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop
        for(int i=0;i<M;i++){
            #pragma acc cache(a[ICE_ENUM2])
            c[i] = a[i] + (real_t)1.0;
        }
    }
    
    for(int i=0;i<M;i++){
        if (fabs(c[i] - (a[i]+(real_t)1.0)) > PRECISION){
            err++;
        }
    }
    
    free(a);
    free(c);
    return err;
}
#endif
#ifndef T5
int test5(void){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        free(a);
        free(c);
        return 1;
    }
    for(int i=0;i<M;i++){
        a[i]=(real_t)i; 
        c[i]=a[i]; 
    }
    #pragma acc data copyin(a[0:M]) copy(c[0:M])
    {
        #pragma acc parallel loop
        for(int i=0;i<M;i++){
            #pragma acc cache(a[ICE_ENUM_LOWER:ICE_LEN4])
            c[i] = c[i] + (real_t)2.0;
        }
    }
    
    for(int i=0;i<M;i++){
        if (fabs(c[i] - (a[i]+(real_t)2.0)) > PRECISION){
            err++;
        }
    }
    
    free(a);
    free(c);
    return err;
}
#endif
#ifndef T6
int test6(void){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        free(a);
        free(c);
        return 1;
    }
    
    for(int i=0;i<M;i++){
        a[i]=(real_t)i;
        c[i]=0; 
    }
    
    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop gang(dim:ICE_ENUM2)
        for(int i=0;i<M;i++){
            c[i] = a[i] * (real_t)2.0;
        }
    }
    
    for(int i=0;i<M;i++){
        if (fabs(c[i] - a[i]*(real_t)2.0) > PRECISION){
            err++;
        }
    }
    
    free(a);
    free(c);
    return err;
}
#endif
int main(void){
    int failcode=0, failed;
#ifndef T1
    failed=0; 
    for(int i=0;i<NUM_TEST_CALLS;i++){
        failed+=test1();
    }
    if(failed){
        failcode|=(1<<0);
    }
#endif
#ifndef T2
    failed=0; 
    for(int i=0;i<NUM_TEST_CALLS;i++){
        failed+=test2(); 
    }
    if(failed){
        failcode|=(1<<1);
    }
#endif
#ifndef T3
    failed=0; 
    for(int i=0;i<NUM_TEST_CALLS;i++){
        failed+=test3(); 
    }
    if(failed){
        failcode|=(1<<2);
    }
#endif
#ifndef T4
    failed=0; 
    for(int i=0;i<NUM_TEST_CALLS;i++){
        failed+=test4(); 
    }
    if(failed){
        failcode|=(1<<3);
    }
#endif
#ifndef T5
    failed=0; 
    for(int i=0;i<NUM_TEST_CALLS;i++){
        failed+=test5(); 
    }
    if(failed){
        failcode|=(1<<4);
    }
#endif
#ifndef T6
    failed=0; 
    for(int i=0;i<NUM_TEST_CALLS;i++){
        failed+=test6(); 
    }
    if(failed){
        failcode|=(1<<5);
    }
#endif
    return failcode;
}
