#ifndef T1
!T1:devonly,kernels,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest
        devtest(1) = .TRUE.

        !$acc enter data copyin(devtest(1:1))
        !$acc kernels
          devtest(1) = .FALSE.
        !$acc end kernels

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc kernels
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = c(x) + a(x) + b(x)
            END DO
          !$acc end kernels
        !$acc end data
        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
!T2:devonly,kernels,V:2.0-2.7
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest
        devtest(1) = .TRUE.

        !$acc enter data copyin(devtest(1:1))
        !$acc kernels
          devtest(1) = .FALSE.
        !$acc end kernels

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        IF (devtest(1)) THEN
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 1

          !$acc enter data copyin(c(1:LOOPCOUNT))
          c = 0
          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc kernels
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = c(x) + a(x) + b(x)
              END DO
            !$acc end kernels
          !$acc end data
          DO x = 1, LOOPCOUNT
            IF (abs(c(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
          !$acc exit data copyout(c(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (a(x) + b(x) + 1)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

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
