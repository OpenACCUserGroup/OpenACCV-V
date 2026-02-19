// acc_integral_constant_expression.cpp
//
// Feature under test (OpenACC 3.4, Section 1.6, Feb 2026):
// - Clause arguments that require an integral-constant-expression accept
//   C++ integral constant expressions (constexpr values and enum values).
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
#include <cstdlib>
#include <cmath>
#include <cstddef>
// C++ integral constant expressions: constexpr + enum
static constexpr int ICE_CONST2 = 2;
static constexpr int ICE_CONST4 = 4;
enum class E2 : int { V = 2 };
static constexpr int ICE_LOWER = 3;

#ifndef T1
//T1:syntax,collapse-clause,runtime,loop,V:3.4-
// collapse(ICE_CONST2) constexpr ICE
int test1(){
    int err = 0;
    const int M = 48, N = 12, MN = M*N;
    real_t *a=(real_t*)std::malloc((size_t)MN*sizeof(real_t));
    real_t *b=(real_t*)std::malloc((size_t)MN*sizeof(real_t));
    real_t *c=(real_t*)std::malloc((size_t)MN*sizeof(real_t));
    if(!a||!b||!c){
        std::free(a);
        std::free(b);
        std::free(c);
        return 1;
    }
    
    for(int i=0;i<MN;i++){ 
        a[i]=(real_t)(i+2); 
        b[i]=(real_t)(i-1);
        c[i]=0;
    }
    
    #pragma acc data copyin(a[0:MN],b[0:MN]) copyout(c[0:MN])
    {
        #pragma acc parallel loop collapse(ICE_CONST2)
        for(int i=0;i<M;i++){
            for(int j=0;j<N;j++){
                int idx = i*N + j;
                c[idx] = a[idx] + b[idx];
            }
        }
    }
    for(int i=0;i<MN;i++){
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
//T2:syntax,tile-clause,runtime,loop,V:3.4-
// tile(ICE_CONST2) constexpr ICE
int test2(){
    int err = 0;
    const int M = 256;
    real_t *a=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        std::free(a);
        std::free(c);
        return 1;
    }
    
    for(int i=0;i<M;i++){
        a[i]=(real_t)i; c[i]=0;
    }
    
    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop tile(ICE_CONST2)
        for(int i=0;i<M;i++){
            c[i] = a[i] * (real_t)2.0;
        }
    }
    
    for(int i=0;i<M;i++){
        if (std::fabs(c[i] - a[i]*(real_t)2.0) > PRECISION){
            err++;
        }
    }
    std::free(a);
    std::free(c);
    return err;
}
#endif
#ifndef T3
//T3:syntax,tile-clause,runtime,loop,V:3.4-
// tile(ICE_CONST2, int(E2::V)) constexpr + enum-class ICE
int test3(){
    int err = 0;
    const int M = 64, N = 40, MN = M*N;
    real_t *a=(real_t*)std::malloc((size_t)MN*sizeof(real_t));
    real_t *b=(real_t*)std::malloc((size_t)MN*sizeof(real_t));
    real_t *c=(real_t*)std::malloc((size_t)MN*sizeof(real_t));
    if(!a||!b||!c){
        std::free(a);
        std::free(b);
        std::free(c);
        return 1;
    }
    
    for(int i=0;i<MN;i++){
        a[i]=(real_t)(i+1);
        b[i]=(real_t)(3*i); 
        c[i]=0; 
    }
    
    #pragma acc data copyin(a[0:MN],b[0:MN]) copyout(c[0:MN])
    {
        #pragma acc parallel loop tile(ICE_CONST2, (int)E2::V)
        for(int i=0;i<M;i++){
            for(int j=0;j<N;j++){
                int idx = i*N + j;
                c[idx] = a[idx] + b[idx];
            }
        }
    }
    for(int i=0;i<MN;i++){
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
#ifndef T4
//T4:syntax,cache-directive,runtime,loop,V:3.4-
// cache element index uses constexpr ICE
int test4(){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        std::free(a);
        std::free(c);
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
            #pragma acc cache(a[ICE_CONST2])
            c[i] = a[i] + (real_t)1.0;
        }
    }
    
    for(int i=0;i<M;i++){
        if (std::fabs(c[i] - (a[i]+(real_t)1.0)) > PRECISION){
            err++;
        }
    }
    
    std::free(a);
    std::free(c);
    return err;
}
#endif
#ifndef T5
//T5:syntax,cache-directive,runtime,loop,V:3.4-
// cache lower:length uses ICE lower + ICE length
int test5(){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        std::free(a);
        std::free(c);
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
            #pragma acc cache(a[ICE_LOWER:ICE_CONST4])
            c[i] = c[i] + (real_t)2.0;
        }
    }
    
    for(int i=0;i<M;i++){
        if (std::fabs(c[i] - (a[i]+(real_t)2.0)) > PRECISION){
            err++;
        }
    }
    
    std::free(a);
    std::free(c);
    return err;
}
#endif
#ifndef T6
//T6:syntax,gang-clause,runtime,loop,V:3.4-
// gang(dim:ICE_CONST2) where dim is an integral constant expression (constexpr), must evaluate to 1..3
// NOTE: Some compilers may not support the 'dim:' keyword form yet; keep this as spec conformance coverage.
int test6(){
    int err = 0;
    const int M = 512;
    real_t *a=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    real_t *c=(real_t*)std::malloc((size_t)M*sizeof(real_t));
    if(!a||!c){
        std::free(a);
        std::free(c);
        return 1;
    }
    
    for(int i=0;i<M;i++){
        a[i]=(real_t)i;
        c[i]=0; 
    }
    
    #pragma acc data copyin(a[0:M]) copyout(c[0:M])
    {
        #pragma acc parallel loop gang(dim:ICE_CONST2)
        for(int i=0;i<M;i++){
            c[i] = a[i] * (real_t)2.0;
        }
    }
    
    for(int i=0;i<M;i++){
        if (std::fabs(c[i] - a[i]*(real_t)2.0) > PRECISION){
            err++;
        }
    }
    
    std::free(a);
    std::free(c);
    return err;
}
#endif
int main(){
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
