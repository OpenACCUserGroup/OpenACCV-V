#include "acc_testsuite.h"
#ifndef T1
//T1:kernels,if,V:1.0-3.2
int test1(){
    int err = 0;
    srand(SEED);
    int data_on_device = 0;
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            b[x] = a[x];
        }
    }

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - a[x]) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:kernels,if,V:2.0-3.2
int test2(){
    int err = 0;
    srand(SEED);
    int data_on_device = 0;
    int * devtest = (int *)malloc(sizeof(int));
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = 0;
    }

    #pragma acc enter data copyin(a[0:n]) create(b[0:n])
    data_on_device = 1;

    #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            b[x] = a[x];
        }
    }
    #pragma acc exit data copyout(b[0:n]) delete(a[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(b[x] - a[x]) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:kernels,if,devonly,V:2.0-3.2
int test3(){
    int err = 0;
    srand(SEED);
    int data_on_device = 0;
    int * devtest = (int *)malloc(sizeof(int));
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    devtest[0] = 1;

    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
	devtest[0] = 0;

    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = 0;
        }

        #pragma acc enter data copyin(a[0:n]) create(b[0:n])
        for (int x = 0; x < n; ++x){
            a[x] = -1;
        }

        #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
        for (int x = 0; x < n; ++x){
            if (fabs(a[x] + 1) > PRECISION){
                err += 1;
            }
            if (fabs(b[x] + 1) > PRECISION){
                err += 1;
            }
        }
        #pragma acc exit data copyout(a[0:n], b[0:n])
        data_on_device = 0;
        for (int x = 0; x < n; ++x){
            if (fabs(a[x] - b[x]) > PRECISION){
                err += 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T4
//T4:kernels,if,devonly,V:2.0-3.2
int test4(){
    int err = 0;
    srand(SEED);
    int data_on_device = 0;
    int * devtest = (int *)malloc(sizeof(int));
    real_t * a = new real_t[n];
    real_t * b = new real_t[n];
    devtest[0] = 1;
    #pragma acc enter data copyin(devtest[0:1])
    #pragma acc parallel present(devtest[0:1])
    {
      devtest[0] = 0;
    }

    if (devtest[0] == 1){
        for (int x = 0; x < n; ++x){
            a[x] = rand() / (real_t)(RAND_MAX / 10);
            b[x] = 0;
        }

        #pragma acc enter data copyin(a[0:n], b[0:n])
        #pragma acc kernels if(data_on_device) present(a[0:n], b[0:n])
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                b[x] = a[x];
            }
        }
        for (int x = 0; x < n; ++x){
            if (fabs(a[x] - b[x]) > PRECISION) {
                err += 1;
            }
        }
        #pragma acc exit data copyout(a[0:n], b[0:n])
        for (int x = 0; x < n; ++x){
            if (fabs(b[x]) > PRECISION && b[x] != a[x]){
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
    return failcode;
}
