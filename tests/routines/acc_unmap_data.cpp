#include "acc_testsuite.h"
#ifndef T1
//T1:runtime,data,executable-data,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *d;
    real_t *e = new real_t[n];

    d = (real_t *)acc_malloc(n * sizeof(real_t));
    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    acc_map_data(c, d, n * sizeof(real_t));

    #pragma acc data copyin(a[0:n], b[0:n]) present(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
    }

    #pragma acc update host(c[0:n])
    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x]))> PRECISION){
            err += 1;
        }
    }

    acc_unmap_data(c);
    acc_free(d);

    return err;
}
#endif

#ifndef T2
//T2:runtime,data,executable-data,construct-independent,V:2.0-2.7
int test2(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *d;
    real_t *e = new real_t[n];

    d = (real_t *)acc_malloc(2 * n * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    acc_map_data(c, d, n * sizeof(real_t));
    acc_map_data(e, &(d[n]), n * sizeof(real_t));

    #pragma acc data copyin(a[0:n], b[0:n]) present(c[0:n], e[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] = a[x] + b[x];
            }
        }
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                e[x] = a[x] * b[x];
            }
        }
    }

    #pragma acc update host(c[0:n])
    #pragma acc update host(e[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - (a[x] + b[x])) > PRECISION){
            err += 1;
        }
        if (fabs(e[x] - (a[x] * b[x])) > PRECISION){
            err += 1;
        }
    }
    acc_unmap_data(c);
    acc_unmap_data(e);
    acc_free(d);

    return err;
}
#endif

#ifndef T3
//T3:runtime,data,executable-data,construct-independent,V:2.0-2.7
int test3(){
    int err = 0;
    real_t *a = new real_t[n];
    real_t *b = new real_t[n];
    real_t *c = new real_t[n];
    real_t *d;
    real_t *e = new real_t[n];

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    d = (real_t *)acc_malloc(n * sizeof(real_t));

    #pragma acc data copyin(a[0:n], b[0:n]) deviceptr(d)
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                d[x] = a[x] + b[x];
            }
        }
    }

    acc_map_data(c, d, n * sizeof(real_t));
    #pragma acc data copyin(a[0:n], b[0:n]) present(c[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                c[x] += a[x] + b[x];
            }
        }
    }

    #pragma acc update host(c[0:n])

    for (int x = 0; x < n; ++x){
        if (fabs(c[x] - 2 * (a[x] + b[x])) > 2 * PRECISION){
            err += 1;
        }
    }

    acc_unmap_data(c);
    acc_free(d);

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
