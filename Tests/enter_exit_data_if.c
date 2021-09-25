#include "acc_testsuite.h"
#ifndef T1
//T1:data,executable-data,construct-independent,if,reference-counting,V:2.5-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    int dev = 1;
    int host = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc enter data copyin(a[0:n], b[0:n]) if(dev)
    #pragma acc data create(a[0:n], b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }
    #pragma acc exit data delete(a[0:n], b[0:n]) if(dev)

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:data,executable-data,construct-independent,if,V:2.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    int dev = 1;
    int host = 0;

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    #pragma acc enter data create(a[0:n], b[0:n]) if(host)
    #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }
    #pragma acc exit data delete(a[0:n], b[0:n]) if(host)

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:data,executable-data,construct-independent,devonly,if,reference-counting,V:2.5-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * b_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    int dev = 1;
    int host = 0;
    int * devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            a_copy[x] = a[x];
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            b_copy[x] = b[x];
            c[x] = 0.0;
        }

        #pragma acc enter data copyin(a[0:n], b[0:n]) if(dev)
        for (int x = 0; x < n; ++x){
            a[x] = 0;
            b[x] = 0;
        }
        #pragma acc data create(a[0:n], b[0:n]) copyout(c[0:n])
        {
            #pragma acc parallel
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] + b[x];
                }
            }
        }
        #pragma acc exit data copyout(a[0:n], b[0:n]) if(dev)

        for (int x = 0; x < n; ++x){
            if (fabs(c[x] - (a_copy[x] + b_copy[x])) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T4
//T4:data,executable-data,if,construct-independent,devonly,V:2.0-2.7
int test4(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * b_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    int dev = 1;
    int host = 0;
    int * devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            c[x] = 0.0;
        }

        #pragma acc enter data copyin(a[0:n], b[0:n]) if(host)
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            a_copy[x] = a[x];
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            b_copy[x] = b[x];
        }
        #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
        {
            #pragma acc parallel
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] + b[x];
                }
            }
        }
        #pragma acc exit data copyout(a[0:n], b[0:n]) if(host)

        for (int x = 0; x < n; ++x){
            if (fabs(c[x] - (a_copy[x] + b_copy[x])) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T5
//T5:data,executable-data,if,construct-independent,devonly,V:2.0-2.7
int test5(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * b_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    int dev = 1;
    int host = 0;
    int * devtest = (int *)malloc(sizeof(int));

    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel
    {
        devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            c[x] = 0.0;
        }

        #pragma acc enter data create(a[0:n], b[0:n]) if(host)
        #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
        {
            #pragma acc parallel
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] + b[x];
                }
            }
        }
        #pragma acc exit data copyout(a[0:n], b[0:n]) if(host)

        for (int x = 0; x < n; ++x){
            if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
                err += 1;
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
#ifndef T5
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x){
        failed = failed + test5();
    }
    if (failed != 0){
        failcode = failcode + (1 << 4);
    }
#endif
    return failcode;
}
