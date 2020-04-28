#ifndef T1
!T1:kernels,private,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y, z, i !Iterators
        INTEGER,DIMENSION(10*LOOPCOUNT):: a !Data
        INTEGER,DIMENSION(10):: b
        INTEGER :: c
        REAL(8),DIMENSION(160*LOOPCOUNT):: random
        REAL(8) :: false_margin
        INTEGER :: errors = 0
        INTEGER :: temp

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(random)
        false_margin = exp(log(.5) / n)
        DO x = 0, 9
          DO y = 1, LOOPCOUNT
            DO z = 1, 16
              IF (random(x * 16 * LOOPCOUNT + (y - 1) * 16 + z - 1) < false_margin) THEN
                temp = 1
                DO i = 1, z
                  temp = temp * 2
                END DO
                a(x * LOOPCOUNT + y) = a(x * LOOPCOUNT + y) + temp
              END IF
            END DO
          END DO
        END DO

        !$acc data copyin(a(1:10*LOOPCOUNT)), copy(b(1:10))
          !$acc kernels loop private(c)
          DO x = 0, 9
            c = a(x * LOOPCOUNT + 1)
            !$acc loop vector reduction(iand:c)
            DO y = 1, LOOPCOUNT
              c = iand(c, a(x * LOOPCOUNT + y))
            END DO
            b(x + 1) = c
          END DO
        !$acc end data

        DO x = 0, 9
          temp = a(x * LOOPCOUNT + 1)
          DO y = 2, LOOPCOUNT
            temp = iand(temp, a(x * LOOPCOUNT + y))
          END DO
          IF (b(x + 1) .ne. temp) THEN
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
