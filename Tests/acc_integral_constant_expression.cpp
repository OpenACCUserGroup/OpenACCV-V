// acc_integral_constant_expression.cpp
//
// Feature under test (OpenACC 3.4, Section 1.6, Feb 2026):
// - Clause arguments that require an integral-constant-expression accept
//   C++ integral constant expressions (constexpr values and enum values).
//
// Notes:
// - T1: collapse() uses a constexpr ICE.
// - T2: tile() uses a constexpr ICE.
// - T3: tile() mixes constexpr + enum ICE.
// - T4: cache() element index uses a constexpr ICE.
// - T5: cache() lower:length slice uses constexpr ICE values.
// - T6: gang(dim:) uses a constexpr ICE (must be 1..3).
//   Some compilers may not support gang(dim:) yet; keep for spec coverage.
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
