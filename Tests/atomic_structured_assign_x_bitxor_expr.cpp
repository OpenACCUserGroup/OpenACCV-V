#include "acc_testsuite.h"
bool is_possible(int* a, int* b, int length, int prev){
    if (length == 0){
        return true;
    }
    int *passed_a = new int[(length - 1)];
    int *passed_b = new int[(length - 1)];
    for (int x = 0; x < length; ++x){
        if (b[x] == prev){
            for (int y = 0; y < x; ++y){
                passed_a[y] = a[y];
                passed_b[y] = b[y];
            }
            for (int y = x + 1; y < length; ++y){
                passed_a[y - 1] = a[y];
                passed_b[y - 1] = b[y];
            }
            if (is_possible(passed_a, passed_b, length - 1, prev ^ a[x])){
                delete[] passed_a;
                delete[] passed_b;
                return true;
            }
        }
    }
    delete[] passed_a;
    delete[] passed_b;
    return false;
}

#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    int *a = new int[n];
    int *b = new int[n];
    int *totals = new int[(n/10 + 1)];
    int *totals_comparison = new int[(n/10 + 1)];
    int *temp_a = new int[10];
    int *temp_b = new int[10];
    int temp_iterator;
    int ab_iterator;

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 8; ++y){
            if (rand()/(real_t)(RAND_MAX) < .933){ //.933 gets close to a 50/50 distribution for a collescence of 10 values
                a[x] += 1<<y;
            }
        }
    }
    for (int x = 0; x < n/10 + 1; ++x){
        totals[x] = 0;
        totals_comparison[x] = 0;
        for (int y = 0; y < 8; ++y){
            totals[x] +=  1<<y;
            totals_comparison[x] += 1<<y;
        }
    }
    for (int x = 0; x < n; ++x){
        b[x] = 0;
    }
    #pragma acc data copyin(a[0:n]) copy(totals[0:n/10 + 1]) copyout(b[0:n])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic capture
                    {
                        b[x] = totals[x/10];
                        totals[x/10] = totals[x/10] ^ a[x];
                    }
            }
        }
    }

    for (int x = 0; x < n; ++x){
        totals_comparison[x/10] ^= a[x];
    }
    for (int x = 0; x < (n/10 + 1); ++x){
        if (totals_comparison[x] != totals[x]){
            err += 1;
            break;
        }
    }
    for (int x = 0; x < n; x = x + 10){
        temp_iterator = 0;
        for (ab_iterator = x; ab_iterator < n && ab_iterator < x + 10; ab_iterator+= 1){
            temp_a[temp_iterator] = a[ab_iterator];
            temp_b[temp_iterator] = b[ab_iterator];
        }
        if (!is_possible(temp_a, temp_b, temp_iterator, 1)){
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
    return failcode;
}
