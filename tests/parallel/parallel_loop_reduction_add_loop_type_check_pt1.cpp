#include "acc_testsuite.h"
#ifndef T1:private,reduction,combined-constructs,loop,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    char * a = new char[10 * n];
    char * b = new char[10 * n];
    char * c = new char[10 * n];
    char * d = new char[10 * n];
    char total = 10;
    char host_total = 10;
    char temp;

    for (int x = 0; x < 10 * n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = rand() / (real_t)(RAND_MAX / 10);
    }

    #pragma acc data copyin(a[0:10*n],b[0:10*n], c[0:10*n]) copyout(d[0:10*n])
    {
        #pragma acc parallel loop gang private(total)
        for (int x = 0; x < 10; ++x) {
            total = 10;
            #pragma acc loop worker reduction(+:total)
            for (int y = 0; y < n; ++y) {
                total += a[x * n + y] + b[x * n + y];
            }
            #pragma acc loop worker
            for (int y = 0; y < n; ++y) {
                d[x * n + y] = c[x * n + y] + total;
            }
        }
    }
    int error_count = 0;
    int total_count = 0;
    for (int x = 0; x < 10; ++x) {
        host_total = 10;
        for (int y = 0; y < n; ++y) {
            host_total += a[x * n + y] + b[x * n + y];
        }
        for (int y = 0; y < n; ++y) {
            temp = host_total + c[x * n + y];
            if (d[x * n + y] != temp) {
                err += 1;
                error_count += 1;
                printf("Error location x: %d \t y: %d\n", x, y); 
                printf("%d != %d\n", d[x * n + y], temp);
            }
            total_count += 1;
        }
    }

    //printf("%d out of %d failed\n", error_count, total_count);

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
    return failcode;
}