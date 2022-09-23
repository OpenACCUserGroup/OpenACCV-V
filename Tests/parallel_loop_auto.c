#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,loop,combined-constructs,syntactic,V:2.0-2.7
//data independent, treated as a independent clause
int test1(){
    int err = 0;
    srand(SEED);
    real_t * value = (real_t *)malloc(n * sizeof(real_t));
    real_t * empty = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        value[x] = rand() / (real_t)(RAND_MAX / 10);
        empty[x] = 0;
    }

    #pragma acc data copyin(value[0:n]) copy(empty[0:n])
    {
      #pragma acc parallel loop auto
      for (int x = 0; x < n; ++x){
        empty[x] = value[x];
      }
    }

    for (int x = 0; x < n; ++x){
      if (fabs(empty[x] - value[x]) > PRECISION){
        err = 1;
      }
    }

    free(value);
    free(empty);

    return err;
}
#endif

#ifndef T2
//T2:parallel,loop,combined-constructs,syntactic,V:2.0-2.7
//data dependent, treated with as a seq clause. Added the num_gangs clause with 1
int test3(){
    int err = 0;
    srand(SEED);
    real_t * device = (real_t *)malloc(n * sizeof(real_t));
    real_t * host = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        device[x] = rand() / (real_t)(RAND_MAX / 10);
        host[x] = device[x];
    }

    #pragma acc data copy(device[0:n])
    {
      #pragma acc parallel loop num_gangs(1) auto
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

    free(device);
    free(host);

    return err;
}
#endif

#ifndef T3
//T3:parallel,loop,combined-constructs,V:2.0-2.7
//data dependent, treated with as a seq clause. 
int test3(){
    int err = 0;
    srand(SEED);
    real_t * device = (real_t *)malloc(n * sizeof(real_t));
    real_t * host = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        device[x] = rand() / (real_t)(RAND_MAX / 10);
        host[x] = device[x];
    }

    #pragma acc data copy(device[0:n])
    {
      #pragma acc parallel loop num_gangs(1) vector worker auto
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

    free(device);
    free(host);

    return err;
}


#endif

int main(){
    int failcode = 0;
    int failed;
#ifndef T1
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed +=  test1();
    }
    if (failed){
        failcode +=  (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test2();
    }
    if (failed){
        failcode += (1 << 1);
    }
#endif
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed += test3();
    }
    if (failed){
        failcode += (1 << 2);
    }
#endif
    return failcode;
}
