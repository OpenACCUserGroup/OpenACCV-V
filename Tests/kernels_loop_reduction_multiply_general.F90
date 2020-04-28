#ifndef T1
!T1:kernels,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        INTEGER :: errors = 0
        REAL(8) :: temp = 1
        REAL(8) :: multiplied_total = 1

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        a = (999.4 + a)/1000
        b = (999.4 + b)/1000

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc kernels loop reduction(*:multiplied_total)
          DO x = 1, LOOPCOUNT
            multiplied_total = multiplied_total * (a(x) + b(x))
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          temp = temp * (a(x) + b(x))
        END DO
        IF (abs(temp - multiplied_total) .gt. ((temp / 2) + (multiplied_total / 2)) * PRECISION) THEN
          WRITE(*, *) temp
          WRITE(*, *) multiplied_total
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
