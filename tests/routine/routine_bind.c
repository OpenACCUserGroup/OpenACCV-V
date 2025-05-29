#include "acc_testsuite.h"
real_t host_function_identifier_named(real_t* a, long long n);
real_t host_function_string_named(real_t* a, long long n);

#pragma acc routine(host_function_identifier_named) vector bind(device_function_identifier_named)
#pragma acc routine(host_function_string_named) vector bind("device_function_string_named")

#pragma acc routine vector bind(device_function_identifier_unnamed)
real_t host_function_identifier_unnamed(real_t* a, long long n){
    real_t returned = 0;
    for (int x = 0; x < n; ++x){
        returned += a[x];
    }
    return returned;
}

real_t device_function_identifier_unnamed(real_t *a, long long n){
    real_t returned = 0;
    #pragma acc loop reduction(-:returned)
    for (int x = 0; x < n; ++x){
        returned -= a[x];
    }
    return returned;
}

#pragma acc routine vector bind("device_function_string_unnamed")
real_t host_function_string_unnamed(real_t* a, long long n){
    real_t returned = 0;
    for (int x = 0; x < n; ++x){
        returned += a[x];
    }
    return returned;
}

real_t device_function_string_unnamed(real_t* a, long long n){
    real_t returned = 0;
    #pragma acc loop reduction(-:returned)
    for (int x = 0; x < n; ++x){
        returned -= a[x];
    }
    return returned;
}

real_t host_function_identifier_named(real_t* a, long long n){
    real_t returned = 0;
    for (int x = 0; x < n; ++x){
        returned += a[x];
    }
    return returned;
}

real_t device_function_identifier_named(real_t* a, long long n){
    real_t returned = 0;
    #pragma acc loop reduction(-:returned)
    for (int x = 0; x < n; ++x){
        returned -= a[x];
    }
    return returned;
}

real_t host_function_string_named(real_t* a, long long n){
    real_t returned = 0;
    for (int x = 0; x < n; ++x){
        returned += a[x];
    }
    return returned;
}

real_t device_function_string_named(real_t* a, long long n){
    real_t returned = 0;
    #pragma acc loop reduction(-:returned)
    for (int x = 0; x < n; ++x){
        returned -= a[x];
    }
    return returned;
}

#ifndef T1
//T1:routine,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t **a = (real_t **)malloc(n * sizeof(real_t *));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    int on_host = (acc_get_device_type() == acc_device_none);
    for (int x = 0; x < n; ++x){
        a[x] = (real_t *)malloc(n * sizeof(real_t));
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:n][0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b[x] = device_function_identifier_unnamed(a[x], n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if ((!on_host) && (fabs(host_function_identifier_unnamed(a[x], n) + b[x]) > PRECISION)){
            err += 1;
            return 1;
        }
        else if ((on_host) && (fabs(host_function_identifier_unnamed(a[x], n) - b[x]) > PRECISION)){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:routine,construct-independent,V:2.0-2.7
int test2(){
    int err = 0;
    srand(SEED);
    real_t **a = (real_t **)malloc(n * sizeof(real_t *));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    int on_host = (acc_get_device_type() == acc_device_none);
    for (int x = 0; x < n; ++x){
        a[x] = (real_t *)malloc(n * sizeof(real_t));
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
    }

    #pragma acc data copyin(a[0:n][0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b[x] = device_function_string_unnamed(a[x], n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (!on_host && fabs(host_function_string_unnamed(a[x], n) + b[x]) > PRECISION){
            err += 1;
        }
        else if (on_host && fabs(host_function_string_unnamed(a[x], n) - b[x]) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T3
//T3:routine,construct-independent,V:2.0-2.7
int test3(){
    int err = 0;
    srand(SEED);
    real_t **a = (real_t **)malloc(n * sizeof(real_t *));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    int on_host = (acc_get_device_type() == acc_device_none);
    for (int x = 0; x < n; ++x){
        a[x] = (real_t *)malloc(n * sizeof(real_t));
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
    }

    #pragma acc data copyin(a[0:n][0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b[x] = device_function_identifier_named(a[x], n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (!on_host && fabs(host_function_identifier_named(a[x], n) + b[x]) > PRECISION){
            err += 1;
        }
        else if (on_host && fabs(host_function_identifier_named(a[x], n) - b[x]) > PRECISION){
            err += 1;
        }
    }

    return err;
}
#endif

#ifndef T4
//T4:routine,construct-independent,V:2.0-2.7
int test4(){
    int err = 0;
    srand(SEED);
    real_t **a = (real_t **)malloc(n * sizeof(real_t *));
    real_t *b = (real_t *)malloc(n * sizeof(real_t));
    int on_host = (acc_get_device_type() == acc_device_none);
    for (int x = 0; x < n; ++x){
        a[x] = (real_t *)malloc(n * sizeof(real_t));
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
        }
    }

    #pragma acc data copyin(a[0:n][0:n]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop gang worker vector
            for (int x = 0; x < n; ++x){
                b[x] = device_function_string_named(a[x], n);
            }
        }
    }

    for (int x = 0; x < n; ++x){
        if (!on_host && fabs(host_function_string_named(a[x], n) + b[x]) > PRECISION){
            err += 1;
        }
        else if (on_host && fabs(host_function_string_named(a[x], n) - b[x]) > PRECISION){
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
