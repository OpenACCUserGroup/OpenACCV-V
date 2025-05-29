#ifndef T1
!T1:parallel,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        REAL(8),DIMENSION(1):: random
        INTEGER :: errors = 0
        REAL(8) :: scalar
        REAL(8) :: scalar_copy

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(random)
        scalar = random(1)
        scalar_copy = scalar
        CALL RANDOM_NUMBER(a)
        b = 0

        !$acc data copyin(a(1:LOOPCOUNT)) copyout(b(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = a(x) + scalar
            END DO
          !$acc end parallel
        !$acc end data
        DO x = 1, LOOPCOUNT
          IF (abs(b(x) - (a(x) + scalar_copy)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO
        IF (abs(scalar_copy - scalar) .gt. PRECISION) THEN
          errors = errors + 1
        END IF

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

      PROGRAM main
        IMPLICIT NONE
        INTEGER :: failcode, testrun
        LOGICAL :: failed
        INCLUDE "acc_testsuite.Fh"
#ifndef T1
        LOGICAL :: test1
#endif
        failed = .FALSE.
        failcode = 0
#ifndef T1
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
