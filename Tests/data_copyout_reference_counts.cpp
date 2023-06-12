#include "acc_testsuite.h"
#ifndef T1
//T1:data,data-region,reference-counting,devonly,construct-independent,V:2.5-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    int * devtest = (int *)malloc(sizeof(int));
    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
      devtest[0] = 0;
    }

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }


    if (devtest[0] == 1) {
        #pragma acc data copyin(c[0:n])
        {
            #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
            {
                #pragma acc parallel
                {
                    #pragma acc loop
                    for (int x = 0; x < n; ++x){
                        c[x] += a[x] + b[x];
                    }
                }
            }
            for (int x = 0; x < n; ++x){
                if (fabs(c[x]) > PRECISION){
                    err += 1;
                    break;
                }
            }
        }
        for (int x = 0; x < n; ++x){
            if (fabs(c[x]) > PRECISION){
                err += 1;
                break;
            }
        }

    }

    return err;
}
#endif

#ifndef T2
//T2:data,data-region,reference-counting,construct-independent,V:2.5-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    int * devtest = (int *)malloc(sizeof(int));
    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
      devtest[0] = 0;
    }

    for (int x = 0; x < n; ++x){
      a[x] = rand() / (real_t)(RAND_MAX / 10);
      b[x] = rand() / (real_t)(RAND_MAX / 10);
      c[x] = 0.0;
    }

    #pragma acc enter data copyin(c[0:n])
    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] += a[x] + b[x];
            }
        }
    }
    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            if (fabs(c[x]) > PRECISION){
                err += 1;
                break;
            }
        }
    }
    #pragma acc exit data copyout(c[0:n])
    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
            break;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:data,data-region,construct-independent,V:2.5-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    real_t * c = new real_t[n];
    int * devtest = (int *)malloc(sizeof(int));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc enter data copyin(c[0:n])
    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] += a[x] + b[x];
            }
        }
        #pragma acc exit data delete(c[0:n])
    }
    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
            break;
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
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif
    return failcode;
}
