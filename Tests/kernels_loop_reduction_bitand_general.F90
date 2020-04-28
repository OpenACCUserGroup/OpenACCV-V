#ifndef T1
!T1:kernels,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y, z !Iterators
        INTEGER,DIMENSION(LOOPCOUNT):: a !Data
        INTEGER :: b
        REAL(8),DIMENSION(16 * LOOPCOUNT):: randoms
        REAL(8) :: false_margin
        INTEGER :: errors = 0
        INTEGER :: temp = 1

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)
        false_margin = exp(log(.5)/n)
        DO x = 1, LOOPCOUNT
          DO y = 0, 15
            IF (randoms(y * LOOPCOUNT + x) .lt. false_margin) THEN
              temp = 1
              DO z = 0, y
                temp = temp * 2
              END DO
              a(x) = a(x) + temp
            END IF
          END DO
        END DO

        b = a(1)

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc kernels loop reduction(iand:b)
          DO x = 1, LOOPCOUNT
            b = iand(b, a(x))
          END DO
        !$acc end data

        temp = a(1)
        DO x = 2, LOOPCOUNT
          temp = iand(a(x), temp)
        END DO
        IF (temp .ne. b) THEN
          WRITE(*, *) temp
          WRITE(*, *) b
          errors = 1
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
