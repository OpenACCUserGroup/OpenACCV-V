#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,loop,tile,reduction,combined-constructs,V:2.0-2.7
int test1(){
  int err = 0;
  srand(SEED);
  real_t * a = (real_t *)malloc(n * sizeof(real_t));
  real_t * b = (real_t *)malloc(n * sizeof(real_t));
  real_t * c = (real_t *)malloc(n * sizeof(real_t));
  real_t * d2 = (real_t *)malloc(n * n * sizeof(real_t));
  real_t * d3 = (real_t *)malloc(n * n * n * sizeof(real_t));
  real_t temp = 0.0;

  for (int x = 0; x < n; ++x){
      a[x] = rand() / (real_t)(RAND_MAX / 10);
      b[x] = rand() / (real_t)(RAND_MAX / 10);
      c[x] = rand() / (real_t)(RAND_MAX / 10);
  }
  #pragma acc data copy(a[0:n], b[0:n], c[0:n], d2[0:n*n])
  {
      #pragma acc kernels loop tile(*, *) reduction(+:temp)
      for (int x = 0; x < n; ++x){
          for (int y = 0; y < n; ++y){
              temp = 0;
              for (int z = 0; z < n; ++z){
                  temp += a[x] + b[y] + c[z];
              }
              d2[x * n + y] = temp;
          }
      }
  }
  for (int x = 0; x < n; ++x){
      for (int y = 0; y < n; ++y){
          temp = 0.0;
          for (int z = 0; z < n; ++z){
              temp += a[x] + b[y] + c[z];
          }
          if (fabs(temp - d2[x * n + y]) > PRECISION * n){
              err += 1;
          }
      }
  }

    return err;
}
#endif

#ifndef T2
//T2:kernels,loop,tile,combined-constructs,V:2.0-2.7
int test2(){
  int err = 0;
  srand(SEED);
  real_t * a = (real_t *)malloc(n * sizeof(real_t));
  real_t * b = (real_t *)malloc(n * sizeof(real_t));
  real_t * c = (real_t *)malloc(n * sizeof(real_t));
  real_t * d2 = (real_t *)malloc(n * n * sizeof(real_t));
  real_t * d3 = (real_t *)malloc(n * n * n * sizeof(real_t));
  real_t temp = 0.0;

  for (int x = 0; x < n; ++x){
      a[x] = rand() / (real_t)(RAND_MAX / 10);
      b[x] = rand() / (real_t)(RAND_MAX / 10);
      c[x] = rand() / (real_t)(RAND_MAX / 10);
  }

  #pragma acc data copyin(a[0:n], b[0:n], c[0:n]) copyout(d3[0:n*n*n])
  {
      #pragma acc kernels loop tile(n/10, n, n*2)
      for (int x = 0; x < n; ++x){
          for (int y = 0; y < n; ++y){
              for (int z = 0; z < n; ++z){
                  d3[(n * n * x) + (n * y) + z] = a[x] + b[x] + c[x];
              }
          }
      }
  }

  for (int x = 0; x < n; ++x){
      for (int y = 0; y < n; ++y){
          for (int z = 0; z < n; ++z){
              if (fabs(d3[(n * n * x) + (n * y) + z] - a[x] - b[x] - c[x]) > PRECISION * 2){
                  err = 1;
              }
          }
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
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test1();
    }
    if (failed != 0){
        failcode = failcode + (1 << 0);
    }
#endif
#ifndef T2
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test2();
    }
    if (failed != 0){
        failcode = failcode + (1 << 1);
    }
#endif
    return failcode;
}
