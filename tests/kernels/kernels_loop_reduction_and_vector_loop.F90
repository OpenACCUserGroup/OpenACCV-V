#ifndef T1
!T1:kernels,private,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        LOGICAL,DIMENSION(10*LOOPCOUNT):: a !Data
        LOGICAL,DIMENSION(10):: b
        LOGICAL,DIMENSION(10):: has_false
        LOGICAL :: temp
        REAL(8) :: false_margin
        REAL(8),DIMENSION(10*LOOPCOUNT) :: randoms
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)
        false_margin = exp(log(.5) / LOOPCOUNT)
        DO x = 1, 10
          has_false(x) = .FALSE.
          b(x) = .TRUE.
        END DO

        DO x = 0, 9
          DO y = 1, LOOPCOUNT
            IF (randoms(x * LOOPCOUNT + y) .lt. false_margin) THEN
              a(x * LOOPCOUNT + y) = .TRUE.
            ELSE
              a(x * LOOPCOUNT + y) = .FALSE.
              has_false(x + 1) = .TRUE.
            END IF
          END DO
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT)) copy(b(1:10))
          !$acc kernels loop private(temp)
          DO x = 0, 9
            temp = .TRUE.
            !$acc loop vector reduction(.AND.:temp)
            DO y = 1, LOOPCOUNT
              temp = temp .AND. a(x * LOOPCOUNT + y)
            END DO
            b(x + 1) = temp
          END DO
        !$acc end data

        DO x = 0, 9
          temp = .FALSE.
          DO y = 1, LOOPCOUNT
            IF (a(x * LOOPCOUNT + y) .eqv. .FALSE.) THEN
              temp = .TRUE.
            END IF
          END DO
          IF (temp .neqv. has_false(x + 1)) THEN
            errors = 1
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
