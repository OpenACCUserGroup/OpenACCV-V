#include "acc_testsuite.h"
#ifndef T1
//T1:parallel,V:1.0-2.7
int test1(){
    int err = 0;
    srand(SEED);
    real_t* a = new real_t[1024];
    real_t* b = new real_t[1024];
    real_t* c = new real_t[1024];

    for(int x = 0; x < 1024; ++x){
        a[x] = rand() / (real_t)(RAND_MAX / 10);
        b[x] = rand() / (real_t)(RAND_MAX / 10);
        c[x] = 0.0;
    }

    #pragma acc data copyin(a[0:1024], b[0:1024]) copy(c[0:1024])
    {
        #pragma acc parallel
        {
            #pragma acc loop
            for (int _0 = 0; _0 < 2; ++_0){
                #pragma acc loop
                for (int _1 = 0; _1 < 2; ++_1){
                    #pragma acc loop
                    for (int _2 = 0; _2 < 2; ++_2){
                        #pragma acc loop
                        for (int _3 = 0; _3 < 2; ++_3){
                            #pragma acc loop
                            for (int _4 = 0; _4 < 2; ++_4){
                                #pragma acc loop
                                for (int _5 = 0; _5 < 2; ++_5){
                                    #pragma acc loop
                                    for (int _6 = 0; _6 < 2; ++_6){
                                        #pragma acc loop
                                        for (int _7 = 0; _7 < 2; ++_7){
                                            #pragma acc loop
                                            for (int _8 = 0; _8 < 2; ++_8){
                                                #pragma acc loop
                                                for (int _9 = 0; _9 < 2; ++_9){
                                                    c[_0*512 + _1*256 + _2*128 + _3*64 + _4*32 + _5*16 + _6*8+ _7*4 + _8*2 + _9] =
                                                    a[_0*512 + _1*256 + _2*128 + _3*64 + _4*32 + _5*16 + _6*8+ _7*4 + _8*2 + _9] +
                                                    b[_0*512 + _1*256 + _2*128 + _3*64 + _4*32 + _5*16 + _6*8+ _7*4 + _8*2 + _9];
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (int x = 0; x < 1024; ++x){
        if(fabs(c[x] - (a[x] +b[x])) > PRECISION){
            err = 1;
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
