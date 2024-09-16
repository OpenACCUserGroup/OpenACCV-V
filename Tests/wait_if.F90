#ifndef T1
!T1:parallel,wait,async,if,V:2.7-3.3
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c, d, e, f
        INTEGER :: errors = 0

        ! Initialization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        CALL RANDOM_NUMBER(d)
        CALL RANDOM_NUMBER(e)
        f = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) create(c(1:LOOPCOUNT), f(1:LOOPCOUNT))
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel

          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              f(x) = d(x) + e(x)
            END DO
          !$acc end parallel

          !$acc update host(c(1:LOOPCOUNT), f(1:LOOPCOUNT)) wait(1, 2) if(.TRUE.)
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (ABS(c(x) - (a(x) + b(x))) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
          IF (ABS(f(x) - (d(x) + e(x))) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .EQ. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END FUNCTION test1
#endif

#ifndef T2
!T2:parallel,wait,async,if,V:2.7-3.3
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c, d, e, f
        INTEGER :: errors = 0

        ! Initialization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        CALL RANDOM_NUMBER(d)
        CALL RANDOM_NUMBER(e)
        f = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) create(c(1:LOOPCOUNT), f(1:LOOPCOUNT))
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel

          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              f(x) = d(x) + e(x)
            END DO
          !$acc end parallel

          !$acc update host(c(1:LOOPCOUNT), f(1:LOOPCOUNT)) wait(1) if(.TRUE.)
          !$acc update host(c(1:LOOPCOUNT), f(1:LOOPCOUNT)) wait(2) if(.TRUE.)
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (ABS(c(x) - (a(x) + b(x))) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
          IF (ABS(f(x) - (d(x) + e(x))) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .EQ. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF
      END FUNCTION test2
#endif

#ifndef T3
!T3:parallel,wait,async,if,V:2.7-3.3
      LOGICAL FUNCTION test3()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c, d, e, f
        INTEGER :: errors = 0

        ! Initialization
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        CALL RANDOM_NUMBER(d)
        CALL RANDOM_NUMBER(e)
        f = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) create(c(1:LOOPCOUNT), f(1:LOOPCOUNT))
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel

          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              f(x) = d(x) + e(x)
            END DO
          !$acc end parallel

          !$acc update host(c(1:LOOPCOUNT), f(1:LOOPCOUNT)) wait(1, 2) if(.FALSE.)
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (c(x) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
          IF (f(x) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .EQ. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
      END FUNCTION test3
#endif

#ifndef T4
!T4:parallel,wait,async,if,V:2.7-3.3
      LOGICAL FUNCTION test4()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c, d, e, f
        INTEGER :: errors = 0

        ! Initialization
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        CALL RANDOM_NUMBER(d)
        CALL RANDOM_NUMBER(e)
        f = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) create(c(1:LOOPCOUNT), f(1:LOOPCOUNT))
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel

          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              f(x) = d(x) + e(x)
            END DO
          !$acc end parallel

          !$acc update host(c(1:LOOPCOUNT), f(1:LOOPCOUNT)) wait(1) if(.FALSE.)
          !$acc update host(c(1:LOOPCOUNT), f(1:LOOPCOUNT)) wait(2) if(.FALSE.)
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (c(x) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
          IF (f(x) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .EQ. 0) THEN
          test4 = .FALSE.
        ELSE
          test4 = .TRUE.
        END IF
      END FUNCTION test4
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
          failed = failed .OR. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif

#ifndef T2
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test2()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 1
          failed = .FALSE.
        END IF
#endif

#ifndef T3
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test3()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 2
          failed = .FALSE.
        END IF
#endif

#ifndef T4
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test4()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 3
          failed = .FALSE.
        END IF
#endif

        CALL EXIT(failcode)
      END PROGRAM main

