! acc_named_constant_data_firstprivate.F90
!
! Feature under test (OpenACC 3.4, Section 1.6):
! - A Fortran named constant (PARAMETER) is a valid "var" and is allowed
!   in data clauses and firstprivate clauses.
!
! Notes:
! - We only use named constants in read-only ways (copyin/create/firstprivate)
!   and never in a way that would require writing back to the constant.

#ifndef T1
!T1:syntax,data-clause,runtime,compute,V:3.4-
! firstprivate with INTEGER named constant
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        INTEGER, PARAMETER :: K = 7

        errors = 0
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT)) firstprivate(K)
          DO i = 1, LOOPCOUNT
            c(i) = a(i) + DBLE(K)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + DBLE(K))) .GT. PRECISION) errors = errors + 1
        END DO

        test1 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T2
!T2:syntax,data-clause,runtime,compute,V:3.4-
! firstprivate with REAL named constant
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        REAL(8), PARAMETER :: ALPHA = 2.5D0

        errors = 0
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT)) firstprivate(ALPHA)
          DO i = 1, LOOPCOUNT
            c(i) = ALPHA * a(i)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (ALPHA * a(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test2 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T3
!T3:syntax,data-clause,runtime,compute,V:3.4-
! named constant appears in a DATA clause (copyin) and is used on device
! (no firstprivate here on purpose)
      LOGICAL FUNCTION test3()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        INTEGER, PARAMETER :: SHIFT = 3

        errors = 0
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT), SHIFT) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            c(i) = a(i) + DBLE(SHIFT)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + DBLE(SHIFT))) .GT. PRECISION) errors = errors + 1
        END DO

        test3 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T4
!T4:syntax,data-clause,runtime,compute,V:3.4-
! firstprivate with a named-constant ARRAY
      LOGICAL FUNCTION test4()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        REAL(8), PARAMETER :: W(2) = (/ 1.25D0, 0.75D0 /)

        errors = 0
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT)) firstprivate(W)
          DO i = 1, LOOPCOUNT
            c(i) = W(1) * a(i) + W(2)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (W(1) * a(i) + W(2))) .GT. PRECISION) errors = errors + 1
        END DO

        test4 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T5
!T5:syntax,data-clause,runtime,compute,V:3.4-
! named-constant ARRAY appears in a DATA clause (copyin) and is used on device
! (no firstprivate here on purpose)
      LOGICAL FUNCTION test5()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        REAL(8), PARAMETER :: BIAS(2) = (/ 0.5D0, 2.0D0 /)

        errors = 0
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT), BIAS) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            c(i) = BIAS(2) * a(i) + BIAS(1)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (BIAS(2) * a(i) + BIAS(1))) .GT. PRECISION) errors = errors + 1
        END DO

        test5 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T6
!T6:syntax,data-clause,runtime,compute,V:3.4-
! named constant in CREATE clause (device allocation only); used on device
      LOGICAL FUNCTION test6()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        INTEGER, PARAMETER :: MULT = 4

        errors = 0
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT)) create(MULT) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT))
          DO i = 1, LOOPCOUNT
            c(i) = DBLE(MULT) * a(i)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (DBLE(MULT) * a(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test6 = (errors .NE. 0)
      END FUNCTION
#endif

      PROGRAM main
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: failcode, testrun
        LOGICAL :: failed
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

        failcode = 0

#ifndef T1
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test1()
        END DO
        IF (failed) failcode = failcode + 2**0
#endif
#ifndef T2
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test2()
        END DO
        IF (failed) failcode = failcode + 2**1
#endif
#ifndef T3
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test3()
        END DO
        IF (failed) failcode = failcode + 2**2
#endif
#ifndef T4
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test4()
        END DO
        IF (failed) failcode = failcode + 2**3
#endif
#ifndef T5
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test5()
        END DO
        IF (failed) failcode = failcode + 2**4
#endif
#ifndef T6
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test6()
        END DO
        IF (failed) failcode = failcode + 2**5
#endif

        CALL EXIT(failcode)
      END PROGRAM
