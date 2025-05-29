#include "acc_testsuite.h"
void declare_deviceptr(real_t *a, real_t *b, real_t *c, real_t *d){
    #pragma acc declare deviceptr(c)
    #pragma acc parallel present(a[0:n], b[0:n], d[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            c[x] = c[x] + a[x] + b[x];
        }
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            d[x] = c[x] * a[x];
        }
    }
}

#ifndef T1
//T1:declare,runtime,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** d = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** devpointer_c = (real_t **)malloc(n * sizeof(real_t *));
    int *devtest = (int *)malloc(sizeof(int));

    for (int x = 0; x < n; ++x){
        a[x] = (real_t *)malloc(n * sizeof(real_t));
        b[x] = (real_t *)malloc(n * sizeof(real_t));
        c[x] = (real_t *)malloc(n * sizeof(real_t));
        d[x] = (real_t *)malloc(n * sizeof(real_t));
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
            b[x][y] = rand() / (real_t)(RAND_MAX / 10);
            c[x][y] = 1;
            d[x][y] = 0;
        }
    }

    #pragma acc data copyin(a[0:n][0:n], b[0:n][0:n]) copy(c[0:n][0:n]) copyout(d[0:n][0:n])
    {
        for (int x = 0; x < n; ++x){
            devpointer_c[x] = acc_deviceptr(c[x]);
            declare_deviceptr(a[x], b[x], devpointer_c[x], d[x]);
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            if (fabs(c[x][y] - (1 + a[x][y] + b[x][y])) > PRECISION){
                err += 1;
            }
            if (fabs(d[x][y] - (a[x][y] * c[x][y])) > PRECISION * 2){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:declare,runtime,construct-independent,devonly,V:2.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** d = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** devpointer_c = (real_t **)malloc(n * sizeof(real_t *));
    int *devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            for (int y = 0; y < n; ++y){
                a[x][y] = rand() / (real_t)(RAND_MAX / 10);
                b[x][y] = rand() / (real_t)(RAND_MAX / 10);
                c[x][y] = 3;
            }
        }

        #pragma acc data copyin(a[0:n][0:n], b[0:n][0:n])
        {
            for (int x = 0; x < n; ++x){
                #pragma acc data copyout(d[x:1][0:n])
                {
                    devpointer_c[x] = acc_copyin(c[x], n * sizeof(real_t));
                    declare_deviceptr(a[x], b[x], devpointer_c[x], d[x]);
                }
                for (int y = 0; y < n; ++y){
                    if (fabs(c[x][y] - 3) > PRECISION){
                        err += 1;
                    }
                }
                for (int y = 0; y < n; ++y){
                    if (fabs(d[x][y] - (a[x][y] * (3 + a[x][y] + b[x][y]))) > PRECISION * 10){
                        err += 1;
                    }
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
