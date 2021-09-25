#include "acc_testsuite.h"
bool possible_result(real_t * remaining_combinations, int length, real_t current_value, real_t test_value){
    if (length == 0){
        if (fabs(current_value - test_value) > PRECISION){
            return true;
        }
        else {
            return false;
        }
    }
    real_t * passed = (real_t *)malloc((length - 1) * sizeof(real_t));
    for (int x = 0; x < length; ++x){
        for (int y = 0; y < x; ++y){
            passed[y] = remaining_combinations[y];
        }
        for (int y = x + 1; y < length; ++y){
            passed[y - 1] = remaining_combinations[y];
        }
        if (possible_result(passed, length - 1, remaining_combinations[x] - current_value, test_value)){
            free(passed);
            return true;
        }
    }
    free(passed);
    return false;
}

#ifndef T1
//T1:atomic,construct-independent,V:2.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t *a = (real_t *)malloc(n * sizeof(real_t));
    real_t *totals = (real_t *)malloc(((n/10) + 1) * sizeof(real_t));
    int indexer = 0;
    real_t * passed = (real_t *)malloc(10 * sizeof(real_t));

    for (int x = 0; x < n; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
    }
    for (int x = 0; x < (n/10) + 1; ++x){
        totals[x] = 0;
    }

    #pragma acc data copyin(a[0:n]) copy(totals[0:(n/10) + 1])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int x = 0; x < n; ++x){
                #pragma acc atomic update
                    totals[x%((int) (n/10) + 1)] = a[x] - totals[x%((int) (n/10) + 1)];
            }
        }
    }
    for (int x = 0; x < (n/10) + 1; ++x){
        indexer = x;
        while (indexer < n){
            passed[indexer/((int) (n/10) + 1)] = a[indexer];
            indexer += (n/10) + 1;
        }
        if (!(possible_result(passed, 10, 0, totals[x]))){
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
