#ifndef T1
!T1:parallel,private,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        LOGICAL,DIMENSION(10*LOOPCOUNT):: a !Data
        LOGICAL,DIMENSION(10) :: b
        LOGICAL :: temp
        REAL(8),DIMENSION(10*LOOPCOUNT):: randoms
        REAL(8) :: false_margin = exp(log(.5) / 2)
        INTEGER :: errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)

        !Initilization
        DO x = 1, 10 * LOOPCOUNT
          IF (randoms(x) > false_margin) THEN
            a(x) = .TRUE.
          ELSE
            a(x) = .FALSE.
          END IF
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT)), copy(b(1:10))
          !$acc parallel loop private(temp)
          DO x = 0, 9
            temp = .FALSE.
            !$acc loop vector reduction(.OR.:temp)
            DO y = 1, LOOPCOUNT
              temp = temp .OR. a(x * LOOPCOUNT + y)
            END DO
            b(x + 1) = temp
          END DO
        !$acc end data

        DO x = 0, 9
          temp = .FALSE.
          DO y = 1, LOOPCOUNT
            temp = temp .OR. a(x * LOOPCOUNT + y)
          END DO
          IF (temp .neqv. b(x + 1)) THEN
            errors = errors + 1
          END IF
        END DO

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
