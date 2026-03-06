! acc_async_wait_argument_compute.F90
!
! Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
! Compute constructs now use the async-argument and wait-argument
! grammar consistently with the rest of the specification.
!
! Tests:
! T1 – Basic async expression and wait.
! T2 – wait(queues: ...) syntax.
! T3 – Default async queue behavior.
! T4 – wait(devnum: ...) syntax.
! T5 – wait(devnum: ... : queues: ...) syntax.
! T6 – async(acc_async_sync).
! T7 – async(acc_async_noval).


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
!T2:wait-argument,syntax,compute-constructs,queues-modifier,V:3.4-
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c, d, e

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0
        d = 0.0D0
        e = 0.0D0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) create(c(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT))

        !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT)) async(1)
        DO i = 1, LOOPCOUNT
          c(i) = a(i) * 2.0D0
        END DO
        !$acc end parallel loop

        !$acc parallel loop present(b(1:LOOPCOUNT), d(1:LOOPCOUNT)) async(2)
        DO i = 1, LOOPCOUNT
          d(i) = b(i) * 3.0D0
        END DO
        !$acc end parallel loop

        ! Spec-valid OpenACC 3.4 wait-argument form under test:
        !$acc parallel loop present(c(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT)) async(3) wait(queues: 1, 2)
        DO i = 1, LOOPCOUNT
          e(i) = c(i) + d(i)
        END DO
        !$acc end parallel loop

        !$acc update self(e(1:LOOPCOUNT)) async(3)
        DO WHILE (.NOT. acc_async_test(3))
        END DO

        DO i = 1, LOOPCOUNT
          IF (ABS(e(i) - (a(i)*2.0D0 + b(i)*3.0D0)) .GT. PRECISION) errors = errors + 1
        END DO

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT), d(1:LOOPCOUNT), e(1:LOOPCOUNT))

        test2 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T3
!T3:async-argument,runtime-api,default-async,compute-constructs,V:3.4-
      LOGICAL FUNCTION test3()
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

        test3 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T4
!T4:wait-argument,syntax,compute-constructs,devnum-prefix,V:3.4-
      LOGICAL FUNCTION test4()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b

        errors = 0
        a = 1.0D0
        b = 0.0D0

        !$acc data copy(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc parallel loop async(1)
          DO i = 1, LOOPCOUNT
            b(i) = a(i) * 2.0D0
          END DO
          !$acc end parallel loop

          ! Spec-valid OpenACC 3.4 wait-argument form under test:
          !$acc parallel loop async(2) wait(devnum: 0 : 1)
          DO i = 1, LOOPCOUNT
            a(i) = b(i) + 1.0D0
          END DO
          !$acc end parallel loop

          !$acc update self(a(1:LOOPCOUNT)) async(2)
          DO WHILE (.NOT. acc_async_test(2))
          END DO
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(a(i) - 3.0D0) .GT. PRECISION) errors = errors + 1
        END DO

        test4 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T5
!T5:wait-argument,syntax,compute-constructs,devnum-queues-prefix,V:3.4-
      LOGICAL FUNCTION test5()
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
          !$acc parallel loop async(1)
          DO i = 1, LOOPCOUNT
            c(i) = a(i) + b(i)
          END DO
          !$acc end parallel loop

          ! Spec-valid OpenACC 3.4 wait-argument combined form under test:
          !$acc parallel loop async(2) wait(devnum: 0 : queues: 1)
          DO i = 1, LOOPCOUNT
            c(i) = c(i) * 2.0D0
          END DO
          !$acc end parallel loop

          !$acc update self(c(1:LOOPCOUNT)) async(2)
          DO WHILE (.NOT. acc_async_test(2))
          END DO
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - 2.0D0*(a(i) + b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test5 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T6
!T6:async-argument,special-value,compute-constructs,acc_async_sync,V:3.4-
      LOGICAL FUNCTION test6()
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

        test6 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T7
!T7:async-argument,special-value,compute-constructs,acc_async_noval,V:3.4-
      LOGICAL FUNCTION test7()
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

        test7 = (errors .NE. 0)
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
#ifndef T5
        LOGICAL :: test5
#endif
#ifndef T6
        LOGICAL :: test6
#endif
#ifndef T7
        LOGICAL :: test7
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
#ifndef T5
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test5()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 4
          failed = .FALSE.
        END IF
#endif
#ifndef T6
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test6()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 5
          failed = .FALSE.
        END IF
#endif
#ifndef T7
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test7()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 6
          failed = .FALSE.
        END IF
#endif

        CALL EXIT(failcode)
      END PROGRAM
