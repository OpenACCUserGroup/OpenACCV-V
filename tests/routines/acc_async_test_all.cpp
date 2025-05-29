#include "acc_testsuite.h"
#ifndef T1
//T1:async,runtime,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    real_t** a = new real_t*[10];
    real_t** b = new real_t*[10];
    real_t** c = new real_t*[10];
    real_t** d = new real_t*[10];
    real_t** e = new real_t*[10];

    for (int x = 0; x < 10; ++x){
        a[x] = new real_t[n];
        b[x] = new real_t[n];
        c[x] = new real_t[n];
        d[x] = new real_t[n];
        e[x] = new real_t[n];
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
            b[x][y] = rand() / (real_t)(RAND_MAX / 10);
            c[x][y] = 0;
            d[x][y] = rand() / (real_t)(RAND_MAX / 10);
            e[x][y] = 0;
        }
    }

    #pragma acc enter data create(a[0:10][0:n], b[0:10][0:n], c[0:10][0:n], d[0:10][0:n], e[0:10][0:n])
    for (int x = 0; x < 10; ++x){
        #pragma acc update device(a[x:1][0:n], b[x:1][0:n], d[x:1][0:n]) async(x)
        #pragma acc parallel present(a[x:1][0:n], b[x:1][0:n], c[x:1][0:n]) async(x)
        {
            #pragma acc loop
            for (int y = 0; y < n; ++y){
                c[x][y] = a[x][y] + b[x][y];
            }
        }
        #pragma acc parallel present(c[x:1][0:n], d[x:1][0:n], e[x:1][0:n]) async(x)
        {
            #pragma acc loop
            for (int y = 0; y < n; ++y){
                e[x][y] = c[x][y] + d[x][y];
            }
        }
        #pragma acc update host(e[x:1][0:n]) async(x)
    }
   
    while(!acc_async_test_all());
    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (fabs(e[x][y] - (a[x][y] + b[x][y] + d[x][y])) > PRECISION){
                err += 1;
            }
        }
    }
    #pragma acc exit data delete(a[0:10][0:n], b[0:10][0:n], c[0:10][0:n], d[0:10][0:n], e[0:10][0:n])
    for (int x = 0; x < 10; ++x){
        delete[] a[x];
        delete[] b[x];
        delete[] c[x];
        delete[] d[x];
        delete[] e[x];
    }
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
    delete[] e;
    
    return err;
}
#endif

#ifndef T2
//T2:async,runtime,construct-independent,V:1.0-2.7
int test2(){
    int err = 0;
    real_t *a = new real_t[10 * n];
    real_t *b = new real_t[10 * n];
    real_t *c = new real_t[10 * n];
    real_t *d = new real_t[10 * n];
    real_t *e = new real_t[10 * n];

    for (int x = 0; x < 10*n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0;
        d[x] = rand() / (real_t)(RAND_MAX / 10);
        e[x] = 0;
    }

    #pragma acc data copyin(a[0:10*n], b[0:10*n], d[0:10*n]) copyout(c[0:10*n], e[0:10*n])
    {
        for (int x = 0; x < 10; ++x){
            #pragma acc parallel present(a[0:10*n], b[0:10*n], c[0:10*n]) async(x)
            {
                #pragma acc loop
                for (int y = 0; y < n; ++y){
                    c[x * n + y] = a[x * n + y] + b[x * n + y];
                }
            }
            #pragma acc parallel present(c[0:10*n], d[0:10*n], e[0:10*n]) async(x)
            {
                #pragma acc loop
                for (int y = 0; y < n; ++y){
                    e[x * n + y] = c[x * n + y] + d[x * n + y];
                }
            }
        }
        while(!acc_async_test_all());
    }

    for (int x = 0; x < 10*n; ++x){
        if (fabs(e[x] - (a[x] + b[x] + d[x])) > PRECISION){
            err += 1;
        }
    }
    
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
    delete[] e;
    
    return err;
}
#endif

#ifndef T3
//T3:async,runtime,construct-independent,V:2.5-2.7
int test3(){
    int err = 0;
    real_t** a = new real_t*[10];
    real_t** b = new real_t*[10];
    real_t** c = new real_t*[10];
    real_t** d = new real_t*[10];
    real_t** e = new real_t*[10];

    for (int x = 0; x < 10; ++x){
        a[x] = new real_t[n];
        b[x] = new real_t[n];
        c[x] = new real_t[n];
        d[x] = new real_t[n];
        e[x] = new real_t[n];
        for (int y = 0; y < n; ++y){
            a[x][y] = rand() / (real_t)(RAND_MAX / 10);
            b[x][y] = rand() / (real_t)(RAND_MAX / 10);
            c[x][y] = 0;
            d[x][y] = rand() / (real_t)(RAND_MAX / 10);
            e[x][y] = 0;
        }
    }

    #pragma acc data copyin(a[0:10][0:n], b[0:10][0:n], d[0:10][0:n]) copyout(c[0:10][0:n], e[0:10][0:n])
    {
        for (int x = 0; x < 10; ++x){
            acc_set_default_async(x);
            #pragma acc parallel present(a[0:10][0:n], b[0:10][0:n], c[0:10][0:n]) async
            {
                #pragma acc loop
                for (int y = 0; y < n; ++y){
                    c[x][y] = a[x][y] + b[x][y];
                }
            }
            #pragma acc parallel present(c[0:10][0:n], d[0:10][0:n], e[0:10][0:n]) async
            {
                #pragma acc loop
                for (int y = 0; y < n; ++y){
                    e[x][y] = c[x][y] + d[x][y];
                }
            }
        }
        while (!acc_async_test_all());
    }

    int count = 0;
    int total = 0;
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < n; ++y) {
            if (fabs(e[x][y]) < PRECISION) {
                count += 1;
            }
            total += 1;
        }
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (fabs(e[x][y] - (a[x][y] + b[x][y] + d[x][y])) > PRECISION){
                err += 1;
            }
        }
    }
    
    for (int x = 0; x < 10; ++x){
        delete[] a[x];
        delete[] b[x];
        delete[] c[x];
        delete[] d[x];
        delete[] e[x];
    }
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
    delete[] e;
    
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
