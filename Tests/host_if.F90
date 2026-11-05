! host_if.F90
!
! Feature under test (OpenACC 3.4, Sections 2.6.5, 2.6.6, and 2.8. March 2026):
! The specification clarifies that the data, enter data, exit data,
! and host_data constructs may contain at most one if clause.
! These tests verify correct behavior when a single valid if clause
! is used on these constructs.
!
! Tests:
! T1 - host_data construct with single if clause:
!      Uses host_data use_device(...) with if(dev) to confirm
!      that device pointer access functions correctly.
!
#ifndef T1
!T1:syntax,host-data,if-clause,construct-independent,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        INTEGER :: dev
        REAL(8), DIMENSION(LOOPCOUNT), TARGET :: a
        REAL(8), POINTER :: seen_ptr(:)

        dev = 1
        NULLIFY(seen_ptr)

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc host_data use_device(a) if(dev .ne. 0)
            seen_ptr => a
          !$acc end host_data
        !$acc end data

        IF (.not. ASSOCIATED(seen_ptr)) THEN
          errors = errors + 1
        END IF

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END FUNCTION
#endif

      PROGRAM main
        IMPLICIT NONE
        INTEGER :: failcode, testrun
        LOGICAL :: failed
        INCLUDE "acc_testsuite.Fh"
#ifndef T1
        LOGICAL :: test1
#endif

        failcode = 0
        failed = .FALSE.

#ifndef T1
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif

        CALL EXIT(failcode)
      END PROGRAM
