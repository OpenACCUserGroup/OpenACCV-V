#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,data,data-region,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * a_host = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        a_host[x] = a[x];
    }

    #pragma acc parallel copy(a[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            a[x] = 2 * a[x];
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(a[x] - (2 * a_host[x])) > PRECISION){
            err = 1;
        }
    }

    return err;
}
#endif
#ifndef T2
//T2:parallel,data,data-region,V:1.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t device = rand() / (real_t)(RAND_MAX / 10);
    real_t host = device;
    #pragma acc parallel loop copy(device) reduction(+:device)
    for(int x = 0; x < n; ++x){
        device += 1.0;
    }

    if(fabs(host - (device - n) ) > PRECISION){
            err++;
    }

    return err;
}
#endif

int main(){
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test1();
    }
    if(failed){
        failcode +=  (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test2();
    }
    if(failed){
        failcode +=  (1 << 1);
    }
#endif

    return failcode;
}