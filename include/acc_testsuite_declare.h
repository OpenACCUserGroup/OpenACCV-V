// #include "acc_testsuite.h"
/* Global headerfile of the OpenACC Testsuite */

/* This file was created with the accts_makeHeader.pl script using the following opions: */
/* -f=accts-c.conf -t=c  */


#ifndef ACC_TESTSUITE_DECLARE_H
#define ACC_TESTSUITE_DECLARE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <complex.h>
#ifdef _OPENACC
#include <openacc.h>
#endif
#include <string.h>
/* Version info                                           */
/**********************************************************/
#define ACCTS_VERSION "2.5"


/* General                                                */
/**********************************************************/
#define ARRAYSIZE   250
long long n = ARRAYSIZE;
#define PRECISION   1e-8
typedef double real_t;

#ifdef DECLARE_TEST
int fixed_size_array[10] = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81};
real_t* datapointer;
int scalar_extern = 10; //For global scalar tests
#endif

#ifdef DECLARE_COPYIN
extern int mult_copyin;
#pragma acc declare copyin(mult_copyin)
#pragma acc routine vector
void extern_multiplyData_copyin(real_t *a, long long n){
    #pragma acc loop
    for (int x = 0; x < n; ++x){
        a[x] = a[x] * mult_copyin;
    }
}
#endif

#ifdef DECLARE_CREATE
extern int mult_create = 2;
#pragma acc declare create(mult_create)

#pragma acc routine vector
void extern_multiplyData(real_t *a){
    #pragma acc data present(a[0:n])
    {
        #pragma acc loop vector
        for (int x = 0; x < n; ++x){
            a[x] = a[x] * 2;
        }
    }

    #pragma acc update host(a[0:n])
}
#endif

#ifdef DECLARE_DEVICE_RESIDENT
extern int mult_device_resident;
#pragma acc declare device_resident(mult_device_resident)
#pragma acc routine vector
void extern_multiplyData_device_resident(real_t *a, long long n){
    #pragma acc loop
    for (int x = 0; x < n; ++x){
        a[x] = a[x] * mult_device_resident;
    }
}
#endif

#ifdef DECLARE_DEVICEPTR
extern real_t* a_deviceptr;
#pragma acc declare deviceptr(a_deviceptr)
#pragma acc routine vector
void extern_multiplyData_deviceptr(int mult, long long n){
    #pragma acc loop
    for (int x = 0; x < n; ++x){
        a_deviceptr[x] = a_deviceptr[x] * mult;
    }
}
#endif

#ifdef __cplusplus
template<typename acctype>
class data_container{
  public:
    acctype* data;
    size_t length;
    inline acctype& operator[](int i){
        return this->data[i];
    }
    acctype* get_data(){
        return this->data;
    }
    data_container(int size){
        this->length = size;
        this->data = (acctype *)malloc(size * sizeof(acctype));
    }
    ~data_container(){
        free(data);
    }
};
#else
#include <stdbool.h>
#endif

#define ARRAYSIZE_NEW 1024
#define ARRAYSIZE_SMALL 10

#define REPETITIONS 1
#define LOOPCOUNT 1000
/* following times are in seconds */
#define SLEEPTIME        0.01
#define SLEEPTIME_LONG   0.5

typedef struct {
  double real;
  double imag;
} dcomplex;


#endif

#ifndef SEED
#define SEED time(NULL)
#endif

#ifndef NUM_TEST_CALLS
#define NUM_TEST_CALLS 1
#endif
// int main(){
//     int failcode = 0;
//     int testrun;
//     int failed;
//     return failcode;
// }