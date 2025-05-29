#include "acc_testsuite.h"
void copyin_copyout_test(real_t *a, real_t *b, real_t *c){
    #pragma acc declare copy(c[0:n])
    #pragma acc parallel present(a[0:n], b[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            c[x] = c[x] + a[x] + b[x];
        }
    }
}

#ifndef T1
//T1:declare,construct-independent,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = (real_t *)malloc(n * sizeof(real_t));
        b[x] = (real_t *)malloc(n * sizeof(real_t));
        c[x] = (real_t *)malloc(n * sizeof(real_t));
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
            b[x][y] = rand() / (real_t)(RAND_MAX / 10);
            c[x][y] = 1;
        }
    }
    #pragma acc data copyin(a[0:n][0:n], b[0:n][0:n])
    {
        for (int x = 0; x < n; ++x){
            copyin_copyout_test(a[x], b[x], c[x]);
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            if (fabs(c[x][y] - (1 + a[x][y] + b[x][y])) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:declare,construct-independent,reference-counting,V:2.5-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
            b[x][y] = rand() / (real_t)(RAND_MAX / 10);
            c[x][y] = 2;
        }
    }

    #pragma acc data copyin(a[0:n][0:n], b[0:n][0:n])
    {
        for (int x = 0; x < n; ++x){
            #pragma acc data copy(c[x:1][0:n])
            {
                copyin_copyout_test(a[x], b[x], c[x]);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            if (fabs(c[x][y] - (2 + a[x][y] + b[x][y])) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:declare,construct-independent,devonly,reference-counting,V:2.5-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t));
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
                #pragma acc data copyin(c[x:1][0:n])
                {
                    copyin_copyout_test(a[x], b[x], c[x]);
                    for (int y = 0; y < n; ++y){
                        if (fabs(c[x][y] - 3) > PRECISION){
                            err += 1;
                        }
                    }
                }
                for (int y = 0; y < n; ++y){
                    if (fabs(c[x][y] - 3) > PRECISION){
                        err += 1;
                    }
                }
            }
        }
    }

    return err;
}
#endif

#ifndef T4
//T4:declare,construct-independent,devonly,reference-counting,V:2.5-2.7
int test4(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t));
    int *devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 0;
    }

    if (devtest[1] == 1){
        for (int x = 0; x < n; ++x){
            for (int y = 0; y < n; ++y){
                a[x][y] = rand() / (real_t)(RAND_MAX / 10);
                b[x][y] = rand() / (real_t)(RAND_MAX / 10);
                c[x][y] = 4;
            }
        }

        #pragma acc data copyin(a[0:n][0:n], b[0:n][0:n])
        {
            for (int x = 0; x < n; ++x){
                #pragma acc data copy(c[x:1][0:n])
                {
                    copyin_copyout_test(a[x], b[x], c[x]);
                    for (int y = 0; y < n; ++y){
                        if (fabs(c[x][y] - 4) > PRECISION){
                            err += 1;
                        }
                    }
                }
                for (int y = 0; y < n; ++y){
                    if (fabs(c[x][y] - (4 + a[x][y] + b[x][y])) > PRECISION){
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
#ifndef T3
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test3();
    }
    if (failed != 0){
        failcode = failcode + (1 << 2);
    }
#endif
#ifndef T4
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test4();
    }
    if (failed != 0){
        failcode = failcode + (1 << 3);
    }
#endif
    return failcode;
}
