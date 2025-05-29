#ifndef T1
!T1:parallel,reduction,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10, LOOPCOUNT):: a !Data
        REAL(8) :: avg
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
 
        !$acc data copy(a(1:10, 1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, 10
              avg = 0.0
              DO WHILE (avg - 1000 .lt. PRECISION * LOOPCOUNT)
                avg = 0.0
                !$acc loop
                DO y = 1, LOOPCOUNT
                  a(x, y) = a(x, y) * 1.5
                END DO
                !$acc loop reduction(+:avg)
                DO y = 1, LOOPCOUNT
                  avg = avg + (a(x, y) / LOOPCOUNT)
                END DO
              END DO
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 1, 10
          avg = 0.0
          DO y = 1, LOOPCOUNT
            avg = avg + (a(x, y) / LOOPCOUNT)
          END DO
          IF (avg < 1000 .OR. avg > 1600) THEN
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
