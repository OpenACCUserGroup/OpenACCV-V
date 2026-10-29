! acc_wait_argument.F90
!
! Feature under test (OpenACC 3.4, Sections 2.5 and 2.16, March 2026):
! Compute constructs now use the async-argument and wait-argument
! grammar consistently with the rest of the specification.
!
! Tests:
! T1 – wait(queues: ...) syntax.
! T2 – wait(devnum: ...) syntax.
! T3 – wait(devnum: ... : queues: ...) syntax.


#ifndef T1
!T1:wait-argument,syntax,compute-constructs,queues-modifier,V:3.4-
      LOGICAL FUNCTION test1()
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

        test1 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T2
!T2:wait-argument,syntax,compute-constructs,devnum-prefix,V:3.4-
      LOGICAL FUNCTION test2()
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

        test2 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T3
!T3:wait-argument,syntax,compute-constructs,devnum-queues-prefix,V:3.4-
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

        test3 = (errors .NE. 0)
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

        CALL EXIT(failcode)
      END PROGRAM
