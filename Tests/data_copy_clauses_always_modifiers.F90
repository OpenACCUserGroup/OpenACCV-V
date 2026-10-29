! data_copy_clauses_always_modifiers.F90
!
! Feature under test (OpenACC 3.4, Sections 2.7.7, 2.7.8, 2.7.9, April 2026):
! Added the always, alwaysin, and alwaysout modifiers to the
! copy, copyin, and copyout data clauses. These modifiers force
! data movement between the host and device even when data is
! already present on the device or still has an active lifetime.
!
! Tests:
! T1 – copy(alwaysin:...): Verifies that when a variable is already
!      present on the device, the alwaysin modifier forces a fresh
!      transfer from the host to the device. The host copy is modified
!      after enter data, and the device computation must use the
!      updated host values.
!
! T2 – copy(alwaysout:...): Verifies that when a variable has an
!      existing device lifetime, the alwaysout modifier forces a
!      transfer from the device back to the host at the end of the
!      structured data region. A host-side check is performed
!      immediately after the region to confirm the update.
!
! T3 – copy(always:...): Verifies that the always modifier forces both
!      transfer-in and transfer-out. The host copy is modified after
!      enter data, the device performs computation, and the final host
!      result must reflect both the updated input and device computation.
!
! T4 – copyin(alwaysin:...): Verifies that alwaysin works with copyin
!      clauses. When a variable is already present on the device, the
!      modifier forces the updated host values to be used on the device.
!
! T5 – copyin(always:...): Verifies that always also forces transfer-in
!      when used with copyin. This test ensures that always behaves like
!      alwaysin for host-to-device movement.
!
! T6 – copyout(alwaysout:...): Verifies that alwaysout forces transfer-out
!      for variables computed on the device, even when they are already
!      present due to an outer data lifetime. A host-side check is performed
!      immediately after the data region to confirm the update occurred
!      before device data is deleted.
!
! T7 – copyout(always:...): Verifies that always also forces transfer-out
!      when used with copyout. This test confirms that always behaves like
!      alwaysout for device-to-host movement.

#ifndef T1
!T1:runtime,data,executable-data,construct-independent,devonly,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c, expected

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        c = 0

        !$acc enter data copyin(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          a(x) = DBLE(x) * 3.0D0
          expected(x) = a(x)
        END DO

        !$acc data copy(alwaysin:a(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            c(x) = a(x)
          END DO
        !$acc end data

        !$acc exit data delete(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (ABS(c(x) - expected(x)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        test1 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T2
!T2:runtime,data,executable-data,construct-independent,devonly,V:3.4-
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, expected

        a = 0.0D0
        DO x = 1, LOOPCOUNT
          expected(x) = DBLE(x + 4)
        END DO

        !$acc enter data create(a(1:LOOPCOUNT))

        !$acc data copy(alwaysout:a(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            a(x) = DBLE(x + 4)
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (ABS(a(x) - expected(x)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        !$acc exit data delete(a(1:LOOPCOUNT))

        test2 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T3
!T3:runtime,data,executable-data,construct-independent,devonly,V:3.4-
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, expected

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)

        !$acc enter data copyin(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          a(x) = DBLE(x + 1)
          expected(x) = 2.0D0 * a(x)
        END DO

        !$acc data copy(always:a(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            a(x) = a(x) * 2.0D0
          END DO
        !$acc end data

        !$acc exit data delete(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (ABS(a(x) - expected(x)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        test3 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T4
!T4:runtime,data,executable-data,construct-independent,devonly,V:3.4-
      LOGICAL FUNCTION test4()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c, expected

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc enter data copyin(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          a(x) = DBLE(9 + x)
          expected(x) = a(x)
        END DO

        !$acc data copyin(alwaysin:a(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            c(x) = a(x)
          END DO
        !$acc end data

        !$acc exit data delete(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (ABS(c(x) - expected(x)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        test4 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T5
!T5:runtime,data,executable-data,construct-independent,devonly,V:3.4-
      LOGICAL FUNCTION test5()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c, expected

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc enter data copyin(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          a(x) = DBLE(100 + 2 * (x - 1))
          expected(x) = a(x)
        END DO

        !$acc data copyin(always:a(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            c(x) = a(x)
          END DO
        !$acc end data

        !$acc exit data delete(a(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (ABS(c(x) - expected(x)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        test5 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T6
!T6:runtime,data,executable-data,construct-independent,devonly,V:3.4-
      LOGICAL FUNCTION test6()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, expected

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        b = 0.0D0

        DO x = 1, LOOPCOUNT
          expected(x) = 3.0D0 * a(x)
        END DO

        !$acc enter data copyin(a(1:LOOPCOUNT)) create(b(1:LOOPCOUNT))

        !$acc data present(a(1:LOOPCOUNT)) copyout(alwaysout:b(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            b(x) = 3.0D0 * a(x)
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (ABS(b(x) - expected(x)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

        test6 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T7
!T7:runtime,data,executable-data,construct-independent,devonly,V:3.4-
      LOGICAL FUNCTION test7()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, expected

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        b = 0.0D0

        DO x = 1, LOOPCOUNT
          expected(x) = a(x) + 7.0D0
        END DO

        !$acc enter data copyin(a(1:LOOPCOUNT)) create(b(1:LOOPCOUNT))

        !$acc data present(a(1:LOOPCOUNT)) copyout(always:b(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            b(x) = a(x) + 7.0D0
          END DO
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (ABS(b(x) - expected(x)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

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
