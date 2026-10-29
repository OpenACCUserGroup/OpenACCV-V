! acc_async_sync.F90
!
! Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
! Compute constructs now use the async-argument and wait-argument
! grammar consistently with the rest of the specification.
!
! Test:
! T1 – async(acc_async_sync).
! T2 – Behavior check: verifies no async clause behaves like async(acc_async_sync).

#ifndef T1
!T1:async-argument,special-value,compute-constructs,acc_async_sync,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop async(acc_async_sync)
          DO i = 1, LOOPCOUNT
            c(i) = a(i) + b(i)
          END DO
          !$acc end parallel loop

          !$acc wait
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test1 = (errors .NE. 0)
      END FUNCTION
#endif
#ifndef T2
!T2:async-argument,special-value,compute-constructs,acc_async_sync,equivalence,V:3.4-
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c, d

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0
        d = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT), d(1:LOOPCOUNT))
          !$acc parallel loop
          DO i = 1, LOOPCOUNT
            c(i) = a(i) + b(i)
          END DO
          !$acc end parallel loop

          !$acc parallel loop async(acc_async_sync)
          DO i = 1, LOOPCOUNT
            d(i) = a(i) + b(i)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
          IF (ABS(d(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
          IF (ABS(c(i) - d(i)) .GT. PRECISION) errors = errors + 1
        END DO

        test2 = (errors .NE. 0)
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
#ifndef T2
        LOGICAL :: test2
#endif

        failcode = 0
        failed = .FALSE.

#ifndef T1
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif
#ifndef T2
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test2()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 1
          failed = .FALSE.
        END IF
#endif

        CALL EXIT(failcode)
      END PROGRAM
