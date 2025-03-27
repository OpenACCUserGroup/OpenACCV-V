#include "acc_testsuite.h"
#ifndef T1
//T1: , V:1.0-3.3
int test1(){
    int err = 0;
    real_t test = 0;
    #pragma acc parallel loop copyout(test) copy(test) reduction(+:test)
    for( int x = 0; x <n; ++x){
        test += 1;
    }

    if(fabs(test - n) > PRECISION){
        err++;
    }

    return err;
}
#endif

#ifndef T2
//T2: , V:1.0-3.3
int test2(){
    int err = 0;
    real_t *test = new real_t[n];

    for(int x = 0; x < n; ++x){
        test[x] = 1.0;
    }

   #pragma acc parallel loop copyout(test[0:n]) copy(test[0:n])
   for(int x = 0; x < n; ++x){
        test[x] += 1.0;
   }

   for(int x = 0; x < n; ++x){
        if(fabs(test[x] - 2.0) > PRECISION){
            err++;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3: , V:1.0-2.7
int test3(){
    int err = 0;
    real_t test = 0;
    #pragma acc parallel loop copy(test) copyout(test) reduction(+:test)
    for( int x = 0; x <n; ++x){
        test += 1;
    }

    if(fabs(test - n) > PRECISION){
        err++;
    }

    return err;
}
#endif

#ifndef T4
//T4: , V:1.0-2.7
int test4(){
    int err = 0;
    real_t *test = new real_t[n];

    for(int x = 0; x < n; ++x){
        test[x] = 1.0;
    }

   #pragma acc parallel loop copy(test[0:n]) copyout(test[0:n])
   for(int x = 0; x < n; ++x){
        test[x] += 1.0;
   }

   for(int x = 0; x < n; ++x){
        if(fabs(test[x] - 2.0) > PRECISION){
            err++;
        }
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for( int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test1();
    }
    if(failed){
        failcode += (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for( int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test2();
    }
    if(failed){
        failcode += (1 << 1);
    }
#endif
#ifndef T3
    failed = 0;
    for( int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test3();
    }
    if(failed){
        failcode += (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for( int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test4();
    }
    if(failed){
        failcode += (1 << 3);
    }
#endif
    return failcode;
}

