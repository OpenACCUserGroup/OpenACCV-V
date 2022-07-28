#ifndef T1
!T1:async,parallel,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10 * LOOPCOUNT):: a, b, c, d !Data
				INTEGER,DIMENSION(10):: errors_array
        INTEGER :: errors

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        d = a + b
				errors = 0

        !$acc data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT), c(1:10*LOOPCOUNT), d(1:10*LOOPCOUNT)) copy(errors_array(1:10))
          DO x = 0, 9
            !$acc parallel loop async(x)
            DO y = 1, LOOPCOUNT
              c(x * LOOPCOUNT + y) = a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y)
            END DO
            !$acc parallel loop async(x) reduction(+:errors_array(x))
            DO y = 1, LOOPCOUNT
              IF (c(x * LOOPCOUNT + y) - d(x * LOOPCOUNT + y) .gt.  PRECISION .OR. &
                d(x * LOOPCOUNT + y) - c(x * LOOPCOUNT + y) .gt. PRECISION) THEN
                errors_array(x) = errors_array(x) + 1
              END IF
            END DO
          END DO
					!$acc wait
        !$acc end data

				DO x = 1, 10
					errors = errors + errors_array(x)
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
