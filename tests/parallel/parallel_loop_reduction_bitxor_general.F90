#ifndef T1
!T1:parallel,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        INTEGER,DIMENSION(LOOPCOUNT):: a !Data
        REAL(8),DIMENSION(LOOPCOUNT):: randoms
        INTEGER :: errors = 0
        INTEGER :: b = 0
        INTEGER :: temp = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)
        a = FLOOR(randoms*1000000)

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc parallel loop reduction(ieor:b)
          DO x = 1, LOOPCOUNT
            b = ieor(b, a(x))
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          temp = ieor(temp, a(x))
        END DO
        IF (temp .ne. b) THEN
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
