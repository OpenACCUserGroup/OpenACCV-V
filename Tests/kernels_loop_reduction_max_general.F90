#ifndef T1
!T1:kernels,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        REAL(8) :: maximum, temp
        INTEGER :: errors = 0

        !Initilization
        maximum = 0
        temp = 0
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc kernels loop reduction(max:maximum)
          DO x = 1, LOOPCOUNT
            maximum = max(maximum, a(x) * b(x))
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          temp = max(temp, a(x) * b(x))
        END DO
        IF (abs(temp - maximum) .gt. PRECISION) THEN
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
