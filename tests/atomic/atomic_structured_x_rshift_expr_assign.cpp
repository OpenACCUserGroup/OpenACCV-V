#include "acc_testsuite.h"
bool is_possible(unsigned int a, unsigned int* b, int length, unsigned int prev){
    if (length == 0){
        return true;
    }
    unsigned int passed_a = 0;
    unsigned int *passed_b = (unsigned int *)malloc((length - 1) * sizeof(unsigned int));
    for (int x = 0; x < length; ++x){
        if ((b[x] == (prev >> 1) && (a>>x)%2==1) || b[x] == prev && (a>>x)%2==0){
            for (int y = 0; y < x; ++y){
                if ((a>>y)%2 == 1){
                    passed_a += 1<<y;
                }
                passed_b[y] = b[y];
            }
            for (int y = x + 1; y < length; ++y){
                if ((a>>y) % 2 == 1){
                    passed_a += 1<<(y - 1);
                }
                passed_b[y - 1] = b[y];
            }
            if (is_possible(passed_a, passed_b, length - 1, b[x])){
                delete[] passed_b;
                return true;
            }
        }
    }
    delete[] passed_b;
    return false;
}

#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    unsigned int *a = (unsigned int *)malloc(n * sizeof(int));
    unsigned int *b = (unsigned int *)malloc(n * sizeof(int));
    unsigned int *c = (unsigned int *)malloc(7 * n * sizeof(int));
    unsigned int passed = 1<<8;

    for (int x = 0; x < n; ++x){
        a[x] = 1<<8;
        for (int y = 0; y < 7; ++y){
            if ((rand()/(real_t) (RAND_MAX)) > .5){
                b[x] += 1<<y;
            }
        }
    }

    #pragma acc data copyin(b[0:n]) copy(a[0:n]) copyout(c[0:7*n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc loop
                for (int y = 0; y < 7; ++y){
                    c[x * 7 + y] = a[x];
                    if ((b[x]>>y)%2 == 1){
                        #pragma acc atomic capture
                        {
                            a[x] = a[x] >> 1;
                            c[x * 7 + y] = a[x];
                        }
                    }
                }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 7; ++y){
            if ((b[x]>>y)%2 == 1){
                a[x] <<= 1;
            }
        }
        if (a[x] != 1<<8){
            err += 1;
        }
    }

    for (int x = 0; x < n; ++x){
        if (!is_possible(b[x], &(c[x * 7]), 7, passed)){
            err++;
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
    return failcode;
}
