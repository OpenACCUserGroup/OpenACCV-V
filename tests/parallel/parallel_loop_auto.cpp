#include "acc_testsuite.h"

#ifndef T1
//T1:parallel,loop,combined-constructs,V:2.0-2.7
//data dependent, treated with as a seq clause. Added the num_gangs clause with 1
int test1(){
    int err = 0;
    srand(SEED);
    real_t * device = new real_t[n];
    real_t * host = new real_t[n];

    for (int x = 0; x < n; ++x){
        device[x] = rand() / (real_t)(RAND_MAX / 10);
        host[x] = device[x];
    }

    #pragma acc data copy(device[0:n])
    {
      #pragma acc parallel loop num_gangs(1) vector_length(1) num_workers(1) auto
      for (int x = 1; x < n; ++x){
        device[x] = device[x - 1] + device[x];
      }
    }

    real_t rolling_total = 0.0;
    for (int x = 0; x < n; ++x){
      rolling_total += host[x];
      if (fabs(rolling_total - device[x]) > PRECISION){
        err = 1;
      }
    }

    delete[] device;
    delete[] host;

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
    if (failed){
        failcode += (1 << 2);
    }
#endif
    return failcode;
}