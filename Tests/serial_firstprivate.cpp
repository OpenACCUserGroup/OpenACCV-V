#include "acc_testsuite.h"
#ifndef T1
//T1:serial,firstprivate,V:2.6-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[10 * n];
    real_t * b = new real_t[10 * n];
    real_t * c = new real_t[10];
    real_t * c_copy = new real_t[10];
    real_t * d = new real_t[10 * n];

    for (int x = 0; x < 10*n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        d[x] = 0.0;
    }
    for (int x = 0; x < 10; ++x){
        c[x] = rand() / (real_t)(RAND_MAX / 10);
        c_copy[x] = c[x];
    }
    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(d[0:10*n])
    {
        #pragma acc serial firstprivate(c[0:n])
        {
            #pragma acc loop gang
            for (int x = 0; x < n; ++x){
                #pragma acc loop worker
                for (int y = 0; y < 10; ++y){
                    d[x * 10 + y] = a[x * 10 + y] + b[x * 10 + y] + c[y];
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 10; ++y){
            if (fabs(d[x * 10 + y] - (a[x * 10 + y] + b[x * 10 + y] + c_copy[y])) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:serial,firstprivate,V:2.6-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = new real_t[10 * n];
    real_t * b = new real_t[10 * n];
    real_t * c = new real_t[10];
    real_t * c_copy = new real_t[10];
    real_t * d = new real_t[10 * n];

    for (int x = 0; x < 10*n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        d[x] = 0.0;
    }
    for (int x = 0; x < 10; ++x){
        c[x] = 0.0;
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n]) copy(d[0:10*n])
    {
        #pragma acc serial firstprivate(c[0:n])
        {
            #pragma acc loop gang independent
            for (int x = 0; x < n; ++x){
                #pragma acc loop worker independent
                for (int y = 0; y < 10; ++y){
                    c[y] = a[x * 10 + y] - b[x * 10 + y];
                }
                #pragma acc loop worker independent
                for (int y = 0; y < 10; ++y){
                    d[x * 10 + y] = a[x * 10 + y] + b[x * 10 + y] + c[y];
                }
            }
        }
    }

    for (int x = 0; x < 10 * n; ++x){
        if (fabs(d[x] - 2 * a[x]) > PRECISION){
            err += 1;
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
