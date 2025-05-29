#ifndef T1
!T1:kernels,private,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y, z !Iterators
        INTEGER,DIMENSION(10 * LOOPCOUNT):: a !Data
        INTEGER,DIMENSION(10) :: b
        REAL(8) :: false_margin
        REAL(8),DIMENSION(160 * LOOPCOUNT) :: randoms
        INTEGER :: errors = 0
        INTEGER :: temp

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)
        false_margin = exp(log(.5) / n)
        DO x = 1, 10 * LOOPCOUNT
          DO y = 1, 16
            IF (randoms(y * 10 * LOOPCOUNT + y - 1) .gt. false_margin) THEN
              temp = 1
              DO z = 1, y
                temp = temp * 2
              END DO
              a(x) = a(x) + temp
            END IF
          END DO
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT)) copy(b(1:10))
          !$acc kernels loop private(temp)
          DO x = 0, 9
            temp = 0
            !$acc loop vector reduction(ior:temp)
            DO y = 1, LOOPCOUNT
              temp = ior(temp, a(x * LOOPCOUNT + y))
            END DO
            b(x + 1) = temp
          END DO
        !$acc end data

        DO x = 0, 9
          temp = 0
          DO y = 1, LOOPCOUNT
            temp = ior(temp, a(x * LOOPCOUNT + y))
          END DO
          IF (temp .ne. b(x + 1)) THEN
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
