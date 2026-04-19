! capture.c
!
! Feature under test (OpenACC 3.4, Sections 2.7.4, 2.7.9, and 2.7.10, April 2026):
! The capture modifier was added to data clauses to specify that a variable
! requires a discrete device-accessible copy, even when the implementation
! might otherwise use shared memory between the host and device.
!
! Tests:
! T1 – copy(capture:...): Verifies that a captured copy is created at the
!      start of a data region. The host modifies the variable after entry,
!      and the device computation must use the original captured values.
! T2 – copyout(capture:...): Verifies that a captured device copy is used
!      during execution and that results are correctly copied back to the
!      host at region exit. Host-side modifications after capture must not
!      affect device computation.
! T3 – create(capture:...): Verifies that a captured device-only copy is
!      created and used across compute regions. The host version of the
!      variable is modified after capture, and the device must use its own
!      independent copy while the host value remains unchanged.

#ifndef T1
!T1:data,structured-data,construct-independent,capture-modifier,V:3.4
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: x

        x = 2

        !$acc data copy(capture:x(1:LOOPCOUNT))
          x = 1

          !$acc parallel loop copy(x(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            x(i) = x(i) + 1
          END DO
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (abs(x(i) - 3.0D0) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END FUNCTION
#endif

#ifndef T2
!T2:data,structured-data,construct-independent,capture-modifier,V:3.4
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a

        a = -1

        !$acc data copyout(capture:a(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            a(i) = 0
          END DO

          DO i = 1, LOOPCOUNT
            a(i) = 5
          END DO

          !$acc parallel loop present(a(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            a(i) = a(i) + 1
          END DO
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (abs(a(i) - 1.0D0) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF
      END FUNCTION
#endif

#ifndef T3
!T3:data,structured-data,construct-independent,capture-modifier,V:3.4
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, a_ref, b

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        a_ref = a
        b = 0

        !$acc data copy(a(1:LOOPCOUNT)) create(capture:b(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            b(i) = 5
          END DO

          !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            a(i) = a(i) + b(i)
          END DO
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (abs(a(i) - a_ref(i)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
          IF (abs(b(i) - 5.0D0) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
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

        CALL EXIT(failcode)
      END PROGRAM
