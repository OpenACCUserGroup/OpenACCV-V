/* Global headerfile of the OpenACC Testsuite */

/* This file was created with the accts_makeHeader.pl script using the following opions: */
/* -f=accts-c.conf -t=c  */


#ifndef ACC_TESTSUITE_H
#define ACC_TESTSUITE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifdef _OPENACC
#include <openacc.h>
#endif
#include <string.h>
/* Version info                                           */
/**********************************************************/
#define ACCTS_VERSION "2.5"


/* General                                                */
/**********************************************************/
#define ARRAYSIZE   1000
long long n = ARRAYSIZE;
#define PRECISION   1e-8
typedef double real_t;
typedef enum { false, true } bool;
#define ARRAYSIZE_NEW 1024
#define ARRAYSIZE_SMALL 10

#define REPETITIONS 1
#define LOOPCOUNT 1000
/* following times are in seconds */
#define SLEEPTIME	 0.01
#define SLEEPTIME_LONG	 0.5

typedef struct {
  double real;
  double imag;
} dcomplex;


#endif
