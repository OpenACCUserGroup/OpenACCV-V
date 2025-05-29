#ifndef T1
!T1:devonly,kernels,if,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest
        LOGICAL:: data_on_device = .FALSE.
        devtest(1) = .TRUE.

        !$acc enter data copyin(devtest(1:1))
        !$acc kernels present(devtest(1:1))
          devtest(1) = .FALSE.
        !$acc end kernels

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        b = 0

        !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc loop
          DO x = 1, LOOPCOUNT
            b(x) = a(x)
          END DO
        !$acc end kernels

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
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
!T2:devonly,kernels,if,V:2.0-2.7
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest
        LOGICAL:: data_on_device = .FALSE.
        devtest(1) = .TRUE.

        !$acc enter data copyin(devtest(1:1))
        !$acc kernels present(devtest(1:1))
          devtest(1) = .FALSE.
        !$acc end kernels

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        b = 0

        data_on_device = .TRUE.
        !$acc enter data copyin(a(1:LOOPCOUNT)) create(b(1:LOOPCOUNT))
        !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc loop
          DO x = 1, LOOPCOUNT
            b(x) = a(x)
          END DO
        !$acc end kernels
        !$acc exit data copyout(b(1:LOOPCOUNT)) delete(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(b(x) - a(x)) .gt. PRECISION) THEN
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
#ifndef T3
!T3:devonly,kernels,if,V:2.0-2.7
      LOGICAL FUNCTION test3()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest
        LOGICAL:: data_on_device = .FALSE.
        devtest(1) = .TRUE.

        !$acc enter data copyin(devtest(1:1))
        !$acc kernels present(devtest(1:1))
          devtest(1) = .FALSE.
        !$acc end kernels

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        IF (devtest(1) .eqv. .TRUE.) THEN
          CALL RANDOM_NUMBER(a)
          b = 0

          !$acc enter data copyin(a(1:LOOPCOUNT)) create(b(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            a(x) = -1
          END DO

          !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = a(x)
            END DO
          !$acc end kernels

          DO x = 1, LOOPCOUNT
            IF (abs(a(x) + 1) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
            IF (abs(b(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO

          !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          data_on_device = .FALSE.

          DO x = 1, LOOPCOUNT
            IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
      END
#endif
#ifndef T4
!T4:devonly,kernels,if,V:2.0-2.7
      LOGICAL FUNCTION test4()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b !Data
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest
        LOGICAL:: data_on_device = .FALSE.
        devtest(1) = .TRUE.

        !$acc enter data copyin(devtest(1:1))
        !$acc kernels present(devtest(1:1))
          devtest(1) = .FALSE.
        !$acc end kernels

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        IF (devtest(1) .eq. 1) THEN
          CALL RANDOM_NUMBER(a)
          b = 0

          !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc kernels if(data_on_device) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = a(x)
            END DO
          !$acc end kernels

          DO x = 1, LOOPCOUNT
            IF (abs(a(x) - b(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO

          !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

          DO x = 1, LOOPCOUNT
            IF (abs(b(x)) .gt. PRECISION) THEN
              IF (abs(b(x) - a(x)) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            END IF
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test4 = .FALSE.
        ELSE
          test4 = .TRUE.
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
#ifndef T3
        LOGICAL :: test3
#endif
#ifndef T4
        LOGICAL :: test4
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
#ifndef T3
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test3()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 2
          failed = .FALSE.
        END IF
#endif
#ifndef T4
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test4()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 3
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
