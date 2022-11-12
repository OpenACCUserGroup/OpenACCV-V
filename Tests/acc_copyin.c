#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    acc_copyin(a, n * sizeof(real_t));
    acc_copyin(b, n * sizeof(real_t));

    #pragma acc data copy(c[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    acc_copyout(a, n * sizeof(real_t));
    acc_copyout(b, n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:runtime,data,executable-data,construct-independent,V:2.0-2.7
int test2(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    acc_copyin(a, n * sizeof(real_t));
    acc_copyin(b, n * sizeof(real_t));

    #pragma acc data copy(c[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data copyout(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }
    return err;
}
#endif

#ifndef T3
//T3:runtime,data,executable-data,devonly,construct-independent,reference-counting,V:2.5-2.7
int test3(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * b_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));
    int * dev_test = (int *)malloc(n * sizeof(int));

    dev_test[0] = 1;
    #pragma acc enter data copyin(dev_test[0:1])
    #pragma acc parallel present(dev_test[0:1])
    {
        dev_test[0] = 0;
    }

    if (dev_test[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            a_copy[x] = a[x];
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            b_copy[x] = b[x];
            c[x] = 0.0;
        }

        acc_copyin(a, n * sizeof(real_t));
        acc_copyin(b, n * sizeof(real_t));

        for (int x = 0; x < n; ++x){
            a[x] = 0;
            b[x] = 0;
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

        acc_copyout(a, n * sizeof(real_t));
        acc_copyout(b, n * sizeof(real_t));

        for (int x = 0; x < n; ++x){
            if (fabs(a[x] - a_copy[x]) > PRECISION){
                err += 1;
            }
            if (fabs(b[x] - b_copy[x]) > PRECISION){
                err += 1;
            }
            if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
                err += 1;
            }
        }
    }
    return err;
}
#endif

#ifndef T4
//T4:runtime,data,executable-data,construct-independent,compatibility-features,V:2.0-2.7
int test4(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    acc_pcopyin(a, n * sizeof(real_t));
    acc_pcopyin(b, n * sizeof(real_t));

    #pragma acc data copy(c[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }
    return err;
}
#endif

#ifndef T5
//T5:runtime,data,executable-data,construct-independent,compatibility-features,V:2.0-2.7
int test5(){
    int err = 0;
    srand(time(NULL));
    real_t * a = (real_t *)malloc(n * sizeof(real_t));
    real_t * b = (real_t *)malloc(n * sizeof(real_t));
    real_t * c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    acc_present_or_copyin(a, n * sizeof(real_t));
    acc_present_or_copyin(b, n * sizeof(real_t));

    #pragma acc data copyout(c[0:n]) present(a[0:n], b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
    }
    return err;
}
#endif

#ifndef T6
//T6:runtime,data,executable-data,construct-independent,reference-counting,V:2.5-2.7
int test6() {
    int err = 0;
    srand(time(NULL));
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));

    for (int x = 0; x < n; ++x) {
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
    }

    acc_copyin(a, n * sizeof(real_t));
    acc_copyin(a, n * sizeof(real_t));
    acc_copyin(b, n * sizeof(real_t));
    acc_copyin(b, n * sizeof(real_t));

    #pragma acc data copyout(c[0:n])
    {
        #pragma acc parallel present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x) {
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc exit data delete(a[0:n], b[0:n])
    #pragma acc exit data delete(a[0:n], b[0:n])

    for (int x = 0; x < n; ++x) {
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION) {
            err += 1;
        }
    }
    return err;
}
#endif

#ifndef T7
//T7:runtime,data,executable-data,construct-independent,reference-counting,devonly,V:2.5-2.7
int test7() {
    int err = 0;
    srand(time(NULL));
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    real_t *a_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t *b_copy = (real_t *)malloc(n * sizeof(real_t));
    real_t *c = (real_t *)malloc(n * sizeof(real_t));
    int * dev_test = (int *)malloc(n * sizeof(int));

    dev_test[0] = 1;
    #pragma acc enter data copyin(dev_test[0:1])
    #pragma acc parallel present(dev_test[0:1])
    {
        dev_test[0] = 0;
    }

    if (dev_test[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = rand() / (real_t)(RAND_MAX / 10);
            a_copy[x] = a[x];
            b_copy[x] = b[x];
            c[x] = 0;
        }
        
        acc_copyin(a, n * sizeof(real_t));
        acc_copyin(b, n * sizeof(real_t));

        for (int x = 0; x < n; ++x){
            a[x] = 0;
            b[x] = 0;
        }

        acc_copyin(a, n * sizeof(real_t));
        acc_copyin(b, n * sizeof(real_t));

        #pragma acc data copyout(c[0:n])
        {
            #pragma acc parallel present(a[0:n], b[0:n])
            {
                #pragma acc loop
                for (int x = 0; x < n; ++x){
                    c[x] = a[x] + b[x];
                }
            }
        }

        #pragma acc exit data delete(a[0:n], b[0:n])
        #pragma acc exit data delete(a[0:n], b[0:n])

        for (int x = 0; x < n; ++x){
            if (fabs(c[x] - (a_copy[x] + b_copy[x])) > PRECISION){
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
#ifndef T6
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed = failed + test6();
    }
    if (failed != 0) {
        failcode = failcode + (1 << 5);
    }
#endif
#ifndef T7
    failed = 0;
    for (int x = 0; x < NUM_TEST_CALLS; ++x) {
        failed = failed + test7();
    }
    if (failed != 0) {
        failcode = failcode + (1 << 6);
    }
#endif
    return failcode;
}
