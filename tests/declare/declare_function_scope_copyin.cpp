#include "acc_testsuite.h"
void function_test(real_t *a, real_t *b, real_t *c){
    #pragma acc declare copyin(a[0:n], b[0:n])
    #pragma acc parallel present(c[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            c[x] = c[x] + a[x] + b[x];
        }
    }
}

void function_test_dev_only(real_t * a, real_t * b, real_t *c){
    #pragma acc declare copyin(a[0:n], b[0:n])
    #pragma acc parallel present(c[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            c[x] = c[x] + a[x] + b[x];
            a[x] = -1;
            b[x] = -1;
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
        a[x] = new real_t[n];
        b[x] = new real_t[n];
        c[x] = new real_t[n];
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
            b[x][y] = rand() / (real_t)(RAND_MAX / 10);
            c[x][y] = 1;
        }
    }

    #pragma acc data copy(c[0:n][0:n])
    {
        for (int x = 0; x < n; ++x){
            function_test(a[x], b[x], c[x]);
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
//T2:declare,construct-independent,devonly,reference-counting,V:2.5-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t));
    int * devtest = (int *)malloc(sizeof(int));
    real_t ** a_host = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** b_host = (real_t **)malloc(n * sizeof(real_t *));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a_host[x] = new real_t[n];
            b_host[x] = new real_t[n];
            for (int y = 0; y < n; ++y){
                a[x][y] = rand() / (real_t)(RAND_MAX / 10);
                a_host[x][y] = a[x][y];
                b[x][y] = rand() / (real_t)(RAND_MAX / 10);
                b_host[x][y] = b[x][y];
                c[x][y] = 0;
            }
        }

        #pragma acc data copy(a[0:n][0:n], b[0:n][0:n], c[0:n][0:n])
        {
            for (int x = 0; x < n; ++x){
                function_test_dev_only(a[x], b[x], c[x]);
            }
        }

        for (int x = 0; x < n; ++x){
            for (int y = 0; y < n; ++y){
                if (fabs(a[x][y] + 1) > PRECISION){
                    err += 1;
                }
                if (fabs(b[x][y] + 1) > PRECISION){
                    err += 1;
                }
                if (fabs(c[x][y] - (a_host[x][y] + b_host[x][y])) > PRECISION){
                    err += 1;
                }
            }
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:declare,construct-independent,devonly,V:2.0-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t ** a = (real_t **)malloc(n * sizeof(real_t));
    real_t ** b = (real_t **)malloc(n * sizeof(real_t));
    real_t ** c = (real_t **)malloc(n * sizeof(real_t));
    int * devtest = (int *)malloc(sizeof(int));
    real_t ** a_host = (real_t **)malloc(n * sizeof(real_t *));
    real_t ** b_host = (real_t **)malloc(n * sizeof(real_t *));

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
                a_host[x][y] = a[x][y];
                b[x][y] = rand() / (real_t)(RAND_MAX / 10);
                b_host[x][y] = b[x][y];
                c[x][y] = 1;
            }
        }

        #pragma acc data copy(c[0:n][0:n])
        {
            for (int x = 0; x < n; ++x){
                function_test_dev_only(a[x], b[x], c[x]);
            }
        }

        for (int x = 0; x < n; ++x){
            for (int y = 0; y < n; ++y){
                if (fabs(a[x][y] - a_host[x][y]) > PRECISION){
                    err += 1;
                }
                if (fabs(b[x][y] - b_host[x][y]) > PRECISION){
                    err += 1;
                }
                if (fabs(c[x][y] - (1 + a[x][y] + b[x][y])) > PRECISION){
                    err += 1;
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
    return failcode;
}
