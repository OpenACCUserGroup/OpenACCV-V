#ifndef T1
!T1:parallel,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        LOGICAL,DIMENSION(LOOPCOUNT):: a !Data
        LOGICAL :: results = .FALSE.
        LOGICAL :: temp = .FALSE.
        REAL(8) :: false_margin = exp(log(.5) / n)
        REAL(8),DIMENSION(LOOPCOUNT):: random
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(random)
        DO x = 1, LOOPCOUNT
          IF (random(x) > false_margin) THEN
            a(x) = .TRUE.
          ELSE
            a(x) = .FALSE.
          END IF
        END DO

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc parallel loop reduction(.OR.:results)
          DO x = 1, LOOPCOUNT
            results = results .OR. a(x)
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          temp = temp .OR. a(x)
        END DO
        IF (temp .neqv. results) THEN
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
