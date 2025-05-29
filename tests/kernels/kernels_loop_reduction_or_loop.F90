#ifndef T1
!T1:kernels,private,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        LOGICAL,DIMENSION(10*LOOPCOUNT):: a, a_copy !Data
        LOGICAL,DIMENSION(10) :: results
        LOGICAL :: temp = .FALSE.
        REAL(8),DIMENSION(10 * LOOPCOUNT) :: random
        REAL(8) :: false_margin = exp(log(.5) / n)
        INTEGER :: errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(random)

        !Initilization
        DO x = 1, 10 * LOOPCOUNT
          IF (random(x) .gt. false_margin) THEN
            a(x) = .TRUE.
          ELSE
            a(x) = .FALSE.
          END IF
          a_copy(x) = a(x)
        END DO

        DO x = 1, 10
          results(x) = .FALSE.
        END DO

        !$acc data copy(a(1:10*LOOPCOUNT), results(1:10))
          !$acc kernels loop gang private(temp)
          DO x = 0, 9
            temp = .FALSE.
            !$acc loop worker reduction(.OR.:temp)
            DO y = 1, LOOPCOUNT
              temp = temp .OR. a(x * LOOPCOUNT + y)
            END DO
            results(x + 1) = temp
            !$acc loop worker
            DO y = 1, LOOPCOUNT
              IF (results(x + 1) .eqv. .TRUE.) THEN
                IF(a(x * LOOPCOUNT + y) .eqv. .TRUE.) THEN
                  a(x * LOOPCOUNT + y) = .FALSE.
                ELSE
                  a(x * LOOPCOUNT + y) = .TRUE.
                END IF
              END IF
            END DO
          END DO
        !$acc end data

        DO x = 0, 9
          temp = .FALSE.
          DO y = 1, LOOPCOUNT
            temp = temp .OR. a_copy(x * LOOPCOUNT + y)
          END DO
          IF (temp .neqv. results(x + 1)) THEN
            errors = errors + 1
          END IF
          DO y = 1, LOOPCOUNT
            IF (temp .eqv. .FALSE.) THEN
              IF (a(x * LOOPCOUNT + y) .neqv. a_copy(x * LOOPCOUNT + y)) THEN
                errors = errors + 1
              END IF
            ELSE
              IF (a(x * LOOPCOUNT + y) .eqv. a_copy(x * LOOPCOUNT + y)) THEN
                errors = errors + 1
              END IF
            END IF
          END DO
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
