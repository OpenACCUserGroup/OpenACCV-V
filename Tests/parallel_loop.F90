#ifndef T1
!T1:parallel,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0

        !Initilization
        a = 0
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        !$acc data copy(a(1:LOOPCOUNT))
          !$acc parallel loop
          DO x = 1, LOOPCOUNT
            a(x) = 1
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - 1) .gt. PRECISION) THEN
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
#ifndef T2
!T2:parallel,loop,V:1.0-2.7
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0

        !Initilization
        a = 0
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(c)

        !$acc data copy(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x) = b(x)
            END DO
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x) = a(x) + c(x)
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - (b(x) + c(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
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
#ifndef T2
        LOGICAL :: test2
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
#ifndef T2
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test2()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 1
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
