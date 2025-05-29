#ifndef T1
!T1:runtime,async,construct-independent,update,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT, 10):: a, b, c, d, e !Data
        INTEGER :: errors
        errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(d)
        c = 0
        e = 0

        !$acc enter data create(a(1:LOOPCOUNT,1:10), b(1:LOOPCOUNT,1:10), c(1:LOOPCOUNT,1:10), d(1:LOOPCOUNT,1:10), e(1:LOOPCOUNT,1:10))
        DO y = 1, 10
          !$acc update device(a(1:LOOPCOUNT,y:y), b(1:LOOPCOUNT,y:y), d(1:LOOPCOUNT,y:y)) async(y)
          !$acc parallel present(a(1:LOOPCOUNT,y:y), b(1:LOOPCOUNT,y:y), c(1:LOOPCOUNT,y:y)) async(y)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x, y) = a(x, y) + b(x, y)
            END DO
          !$acc end parallel
          !$acc parallel present(c(1:LOOPCOUNT,y:y), d(1:LOOPCOUNT,y:y),e(1:LOOPCOUNT,y:y)) async(y)
            !$acc loop
            DO x = 1, LOOPCOUNT
              e(x, y) = c(x, y) + d(x, y)
            END DO
          !$acc end parallel
          !$acc update host(e(1:LOOPCOUNT,y:y)) async(y)
        END DO

        DO WHILE (acc_async_test_all() .eqv. .FALSE.)
          CONTINUE
        END DO

        DO y = 1, 10
          DO x = 1, LOOPCOUNT
            IF (abs(e(x, y) - (a(x, y) + b(x, y) + d(x, y))) .GT. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END DO

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

#ifndef T2
!T2:async,runtime,construct-independent,V:1.0-2.7
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT, 10):: a, b, c, d, e !Data
        INTEGER :: errors
        errors = 0

        !Initialization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(d)
        c = 0
        e = 0

        !$acc data copyin(a(1:LOOPCOUNT, 1:10), b(1:LOOPCOUNT,1:10), d(1:LOOPCOUNT, 1:10)) copyout(c(1:LOOPCOUNT, 1:10), e(1:LOOPCOUNT, 1:10))
          DO x = 1, 10
            !$acc parallel present(a(1:LOOPCOUNT, 1:10), b(1:LOOPCOUNT, 1:10), c(1:LOOPCOUNT, 1:10)) async(x)
              !$acc loop
              DO y = 1, LOOPCOUNT
                c(y, x) = a(y, x) + b(y, x)
              END DO
            !$acc end parallel
            !$acc parallel present(c(1:LOOPCOUNT, 1:10), d(1:LOOPCOUNT, 1:10), e(1:LOOPCOUNT, 1:10)) async(x)
              !$acc loop
              DO y = 1, LOOPCOUNT
                e(y, x) = c(y, x) + d(y, x)
              END DO
            !$acc end parallel
          END DO
          DO WHILE (acc_async_test_all() .eqv. .FALSE.)
            CONTINUE
          END DO
        !$acc end data

        DO x = 1, 10
          DO y = 1, LOOPCOUNT
            IF (abs(e(y, x) - (a(y, x) + b(y, x) + d(y, x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END DO

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF
      END
#endif

#ifndef T3
!T3:async,runtime,construct-independent,V:2.5-2.7
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT, 10):: a, b, c, d, e !Data
        INTEGER :: errors
        errors = 0

        !Initialization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(d)
        c = 0
        e = 0

        !$acc data copyin(a(1:LOOPCOUNT, 1:10), b(1:LOOPCOUNT, 1:10), d(1:LOOPCOUNT, 1:10)) copyout(c(1:LOOPCOUNT, 1:10), e(1:LOOPCOUNT, 1:10))
          DO x = 1, 10
            CALL acc_set_default_async(x)
            !$acc parallel present(a(1:LOOPCOUNT, 1:10), b(1:LOOPCOUNT, 1:10), c(1:LOOPCOUNT, 1:10)) async
              !$acc loop
              DO y = 1, LOOPCOUNT
                c(y, x) = a(y, x) + b(y, x)
              END DO
            !$acc end parallel
            !$acc parallel present(c(1:LOOPCOUNT, 1:10), d(1:LOOPCOUNT, 1:10), e(1:LOOPCOUNT, 1:10)) async
              !$acc loop
              DO y = 1, LOOPCOUNT
                e(y, x) = c(y, x) + d(y, x)
              END DO
            !$acc end parallel
          END DO
          DO WHILE (acc_async_test_all() .eqv. .FALSE.)
            CONTINUE
          END DO
        !$acc end data

        DO x = 1, 10
          DO y = 1, LOOPCOUNT
            IF (abs(e(y, x) - (a(y, x) + b(y, x) + d(y, x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END DO

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
      END
#endif

      PROGRAM main
      IMPLICIT NONE
      INTEGER :: failcode, testrun
      LOGICAL :: failed
      INCLUDE "acc_testsuite.Fh"
      !Conditionally define test functions
#ifndef T1
      LOGICAL :: test1
#endif
#ifndef T2
      LOGICAL :: test2
#endif
#ifndef T3
      LOGICAL :: test3
#endif
      failcode = 0
      failed = .FALSE.

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
      CALL EXIT (failcode)
      END PROGRAM
