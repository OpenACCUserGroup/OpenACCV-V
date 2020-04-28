#ifndef T1
!T1:runtime,async,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, e !Data
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
        
        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) create(c(1:LOOPCOUNT)) async(1)
        !$acc enter data copyin(d(1:LOOPCOUNT)) create(e(1:LOOPCOUNT)) async(2)
        !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async(1)
          !$acc loop
          DO x = 1, LOOPCOUNT
            c(x) = a(x) + b(x)
          END DO
        !$acc end parallel
        !$acc parallel present(c(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) async(1) wait(2)
          !$acc loop
          DO x = 1, LOOPCOUNT
            e(x) = c(x) + d(x)
          END DO
        !$acc end parallel
        !$acc exit data copyout(e(1:LOOPCOUNT)) async(1)


        DO WHILE (acc_async_test(1) .eqv. .FALSE.)
          CONTINUE
        END DO

        DO x = 1, LOOPCOUNT
          IF (abs(e(x) - (a(x) + b(x) + d(x))) .GT. PRECISION) THEN
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
!T2:async,runtime,construct-independent,V:1.0-2.7
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, e
        INTEGER :: errors
        errors = 0
        
        !Initialization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(d)
        c = 0
        e = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT), e(1:LOOPCOUNT))
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
          !$acc parallel present(c(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              e(x) = c(x) + d(x)
            END DO
          !$acc end parallel
          DO WHILE (acc_async_test(1) .eqv. .FALSE.)
            CONTINUE
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(e(x) - (a(x) + b(x) + d(x))) .GT. PRECISION) THEN
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
!T3:runtime,async,construct-independent,V:2.5-2.7
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, e
        INTEGER :: errors, async_val
        INTEGER :: acc_get_default_async
        async_val = acc_get_default_async()
        errors = 0

        !Initialization
        CALL RANDOM_SEED()
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(d)
        c = 0
        e = 0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), d(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT), e(1:LOOPCOUNT))
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
          !$acc parallel present(c(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) async
            !$acc loop
            DO x = 1, LOOPCOUNT
              e(x) = c(x) + d(x)
            END DO
          !$acc end parallel
          DO WHILE (acc_async_test(async_val) .eqv. .FALSE.)
            CONTINUE
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(e(x) - (a(x) + b(x) + d(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
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
