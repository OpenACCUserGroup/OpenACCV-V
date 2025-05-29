/* Global headerfile of the OpenACC Testsuite */

/* This file was created with the accts_makeHeader.pl script using the following opions: */
/* -f=accts-c.conf -t=c  */


#ifndef ACC_TESTSUITE_H
#define ACC_TESTSUITE_H

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
#define ARRAYSIZE   100
long long n = ARRAYSIZE;
#define PRECISION   1e-8
typedef double real_t;

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

#define ARRAYSIZE_NEW 256
#define ARRAYSIZE_SMALL 10

#define REPETITIONS 1
#define LOOPCOUNT 100
/* following times are in seconds */
#define SLEEPTIME	 0.01
#define SLEEPTIME_LONG	 0.5

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

typedef struct {
  real_t * a;
  real_t * b;
} two_d_array;
