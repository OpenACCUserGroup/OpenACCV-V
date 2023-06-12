#include "acc_testsuite.h"

#ifndef T1
//T1:parallel,loop,reduction,combined-constructs,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    char * a = new char[10 * n];
    char * a_copy = new char[10 * n];
    char * has_false = new char[10];
    real_t false_margin = pow(exp(1), log(.5)/n);
    char temp = 1;

    for (int x = 0; x < 10; ++x){
        has_false[x] = 0;
    }

    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (rand() / (real_t)(RAND_MAX) < false_margin){
                a[x * n + y] = 1;
                a_copy[x * n + y] = 1;
            }
            else {
                a[x * n + y] = 0;
                a_copy[x * n + y] = 0;
                has_false[x] = 1;
            }
        }
    }

    #pragma acc data copy(a[0:10*n])
    {
        #pragma acc parallel loop gang private(temp)
        for (int x = 0; x < 10; ++x){
            temp = 1;
            #pragma acc loop worker reduction(&&:temp)
            for (int y = 0; y < n; ++y){
                temp = temp && a[x * n + y];
            }
            #pragma acc loop worker
            for (int y = 0; y < n; ++y){
                a[x * n + y] = a[x * n + y] ^ temp;
            }
        }
    }


    for (int x = 0; x < 10; ++x){
        for (int y = 0; y < n; ++y){
            if (has_false[x] == 1 && a[x * n + y] != a_copy[x * n + y]){
                err = 1;
            }
            else if (has_false[x] == 0 && a[x * n + y] == a_copy[x * n + y]){
                err = 1;
            }
        }
    }

    return err;
}
#endif

#ifndef T2
//T2:parallel,private,reduction,combined-constructs,loop,V:2.7-2.7
int test2(){
    int err = 0;
    srand(SEED);
    char * a = new char[n * 25];
    char * a_copy = new char[n * 25];
    char * has_false = new char[25];
    real_t false_margin = pow(exp(1), log(.5)/n);
    char device[5];
    char host[5];

    for (int x = 0; x < 5; ++x) {
        device[x] = 1;
        host[x] = 1;
        for (int y = 0; y < 5; ++y) {
            has_false[x * 5 + y] = 0;
        }
    }

    for (int x = 0; x < 25 * n; ++x){
        if (rand() / (real_t)(RAND_MAX) < false_margin) {
            a[x] = 1;
            a_copy[x] = 1;
        }
        else {
            a[x] = 0;
            a_copy[x] = 0;
            has_false[x % 25] = 1;
        }
    }

    #pragma acc data copy(a[0:25*n])
    {
        #pragma acc parallel loop gang private(device)
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                device[y] = 1;
            }
            #pragma acc loop worker reduction(&&:device)
            for (int y = 0; y < 5 * n; ++y) {
                device[y%5] = device[y%5] && a[x * 5 * n + y];
            }
            #pragma acc loop worker
            for (int y = 0; y < 5 * n; ++y) {
                a[x * 5 * n + y] = a[x * 5 * n + y] ^ device[y%5];
            }
        }
    }

    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            host[y] = 1;
        }
        for (int y = 0; y < 5 * n; ++y) {
            host[y%5] = host[y%5] && a_copy[x * 5 * n + y];
        }
        for (int y = 0; y < 5 * n; ++y) {
            if (a[x * 5 * n + y] != (a_copy[x * 5 * n + y] ^ host[y % 5])) {
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
    return failcode;
}
