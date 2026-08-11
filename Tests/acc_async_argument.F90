! acc_async_argument.F90
!
! Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
! Compute constructs now use the async-argument and wait-argument
! grammar consistently with the rest of the specification.
!
! Tests:
! T1 – Basic async expression and wait.
! T2 – Default async queue behavior.
! T3 – async(acc_async_sync).
! T4 – async(acc_async_noval).


#ifndef T1
!T1:async,wait,runtime,compute-constructs,baseline,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        INTEGER :: q, q0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0

        q  = 4
        q0 = q - q   ! nonnegative scalar integer expression (evaluates to 0)

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async(q0)
          DO i = 1, LOOPCOUNT
            c(i) = a(i) + b(i)
          END DO
          !$acc end parallel loop

          !$acc wait
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test1 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T2
!T2:async-argument,runtime-api,default-async,compute-constructs,V:3.4-
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        INTEGER :: q
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

        errors = 0
        q = 7

        ! Spec intent: set and use the default async queue.
        ! Some compilers may not provide this routine in the OPENACC module/runtime.
        CALL acc_set_default_async(q)

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) create(c(1:LOOPCOUNT))

        ! "async" with no argument => use default async queue (spec behavior)
        !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async
        DO i = 1, LOOPCOUNT
          c(i) = a(i) - b(i)
        END DO
        !$acc end parallel loop

        IF (acc_get_default_async() .NE. q) errors = errors + 1

        !$acc update self(c(1:LOOPCOUNT)) async(q)
        DO WHILE (.NOT. acc_async_test(q))
        END DO

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) - b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))

        test2 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T3
!T3:async-argument,special-value,compute-constructs,acc_async_sync,V:3.4-
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop async(acc_async_sync)
          DO i = 1, LOOPCOUNT
            c(i) = a(i) + b(i)
          END DO
          !$acc end parallel loop

          !$acc wait
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test3 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T4
!T4:async-argument,special-value,compute-constructs,acc_async_noval,V:3.4-
      LOGICAL FUNCTION test4()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop async(acc_async_noval)
          DO i = 1, LOOPCOUNT
            c(i) = a(i) - b(i)
          END DO
          !$acc end parallel loop

          !$acc wait
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) - b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test4 = (errors .NE. 0)
      END FUNCTION
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

        failcode = 0
        failed = .FALSE.

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
      END PROGRAM
