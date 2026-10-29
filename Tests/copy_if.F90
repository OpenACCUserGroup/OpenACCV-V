! copy_if.F90
!
!Feature under test (OpenACC 3.4, Sections 2.6.5, 2.6.6, and 2.8. March 2026):
! The specification clarifies that the data,
! and host_data constructs may contain at most one if clause.
! These tests verify correct behavior when a single valid if clause
! is used on these constructs.
!
! Tests:
! T1 – data construct with single if clause:
!      Uses a data region with copy and a single if(dev)
!      clause controlling device execution.
!


#ifndef T1
!T1:syntax,data,if-clause,construct-independent,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        INTEGER :: dev
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b

        dev = 1
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        b = 0.0

        !$acc data copy(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev .ne. 0)
          !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            b(x) = a(x) * 2.0
          END DO
          !$acc end parallel loop
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(b(x) - (a(x) * 2.0)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END FUNCTION
#endif
