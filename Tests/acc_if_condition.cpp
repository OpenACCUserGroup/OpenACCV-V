// acc_if_condition_all.cpp
#include "acc_testsuite.h"
#include <openacc.h>
#include <cstdlib>
#include <cmath>
#include <cstddef>

struct BoolLike {
    int v;
    explicit operator bool() const { return v != 0; }
};

#ifndef T1
//T1:syntax,if-clause,runtime,enter-data,V:3.4-
int test1(){
    int err=0;
    real_t* a=(real_t*)std::malloc(n*sizeof(real_t));
    if(!a) return 1;
    for(int i=0;i<n;++i) a[i]=(real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(false)
    if(acc_is_present(a, (size_t)n*sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(true)
    std::free(a);
    return err;
}
#endif

#ifndef T2
//T2:syntax,if-clause,runtime,enter-data,V:3.4-
int test2(){
    int err=0;
    real_t* a=(real_t*)std::malloc(n*sizeof(real_t));
    if(!a) return 1;
    for(int i=0;i<n;++i) a[i]=(real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(true)
    if(!acc_is_present(a, (size_t)n*sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(true)
    std::free(a);
    return err;
}
#endif

#ifndef T3
//T3:syntax,if-clause,runtime,exit-data,V:3.4-
int test3(){
    int err=0;
    real_t* a=(real_t*)std::malloc(n*sizeof(real_t));
    if(!a) return 1;
    for(int i=0;i<n;++i) a[i]=(real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(true)
    if(!acc_is_present(a, (size_t)n*sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(false)
    if(!acc_is_present(a, (size_t)n*sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(true)
    std::free(a);
    return err;
}
#endif

#ifndef T4
//T4:syntax,if-clause,runtime,exit-data,V:3.4-
int test4(){
    int err=0;
    real_t* a=(real_t*)std::malloc(n*sizeof(real_t));
    if(!a) return 1;
    for(int i=0;i<n;++i) a[i]=(real_t)i;

    #pragma acc enter data copyin(a[0:n]) if(true)
    if(!acc_is_present(a, (size_t)n*sizeof(real_t))) err++;

    #pragma acc exit data delete(a[0:n]) if(true)
    if(acc_is_present(a, (size_t)n*sizeof(real_t))) err++;

    std::free(a);
    return err;
}
#endif

#ifndef T5
//T5:syntax,if-clause,runtime,compute,V:3.4-
// int expression condition (valid C++)
int test5(){
    int err=0;
    std::srand(SEED);
    real_t* a=(real_t*)std::malloc(n*sizeof(real_t));
    real_t* b=(real_t*)std::malloc(n*sizeof(real_t));
    real_t* c=(real_t*)std::malloc(n*sizeof(real_t));
    if(!a||!b||!c){ std::free(a); std::free(b); std::free(c); return 1; }

    for(int i=0;i<n;++i){ a[i]=std::rand()/(real_t)(RAND_MAX/10); b[i]=std::rand()/(real_t)(RAND_MAX/10); c[i]=0; }

    int cond_int = (n > 0);

    #pragma acc data copyin(a[0:n],b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n],b[0:n],c[0:n]) if(cond_int)
        for(int i=0;i<n;++i) c[i]=a[i]+b[i];
    }

    for(int i=0;i<n;++i) if(std::fabs(c[i]-(a[i]+b[i]))>PRECISION) err++;
    std::free(a); std::free(b); std::free(c);
    return err;
}
#endif

#ifndef T6
//T6:syntax,if-clause,runtime,compute,V:3.4-
// C++ condition forms: pointer + user-defined bool-convertible type + fp comparison->bool
int test6(){
    int err=0;
    std::srand(SEED);
    real_t* a=(real_t*)std::malloc(n*sizeof(real_t));
    real_t* c=(real_t*)std::malloc(n*sizeof(real_t));
    if(!a||!c){ std::free(a); std::free(c); return 1; }

    for(int i=0;i<n;++i){ a[i]=std::rand()/(real_t)(RAND_MAX/10); c[i]=0; }

    void*   cond_ptr = (void*)a;       // non-null pointer => true
    BoolLike cond_obj{1};              // operator bool() => true
    double  x = 1.0;
    bool    cond_fp = (x != 0.0);      // C++-correct “float-based” condition

    #pragma acc data copyin(a[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel loop present(a[0:n],c[0:n]) if(cond_obj)
        for(int i=0;i<n;++i) c[i]=a[i]*(real_t)2.0;

        #pragma acc parallel loop present(a[0:n],c[0:n]) if(cond_ptr)
        for(int i=0;i<n;++i) c[i]=c[i]+(real_t)1.0;

        #pragma acc parallel loop present(a[0:n],c[0:n]) if(cond_fp)
        for(int i=0;i<n;++i) c[i]=c[i]+(real_t)1.0;
    }

    for(int i=0;i<n;++i){
        real_t expect = (a[i]*(real_t)2.0) + (real_t)2.0;
        if(std::fabs(c[i]-expect)>PRECISION) err++;
    }

    std::free(a); std::free(c);
    return err;
}
#endif

int main(){
    int failcode=0, failed;
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
    return failcode;
}
