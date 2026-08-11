! acc_expr_eval_directive_args.F90
!
! Feature under test (OpenACC 3.4, Section 2.1, Feb 2026):
! - Clarified user-visible behavior of evaluation of expressions in directive arguments.
!   A program must not depend on the order/number of evaluations of expressions in
!   clause/construct/directive arguments, nor on any side effects of those evaluations.
!
! Notes:
! - T1: uses complex but side-effect-free expressions in directive arguments; checks correctness.
! - T2: uses update ... if_present where data is not present; must be a no-op and not crash.
!       The section length expression may be elided; we do NOT assert call count.
! - T3: uses side-effecting function calls in directive arguments; we do NOT rely on side effects;
!       checks correctness only.

MODULE acc_expr_eval_helpers
  IMPLICIT NONE
  INTEGER, SAVE :: size_calls = 0
  INTEGER, SAVE :: sidefx_calls = 0
CONTAINS

  INTEGER FUNCTION foo_pure(x)
    IMPLICIT NONE
    INTEGER, INTENT(IN) :: x
    foo_pure = MOD(x, 64) + 1
  END FUNCTION foo_pure

  INTEGER FUNCTION bar_pure(x)
    IMPLICIT NONE
    INTEGER, INTENT(IN) :: x
    bar_pure = MOD(x, 32) + 1
  END FUNCTION bar_pure

  INTEGER FUNCTION size_maybe_elided(nval)
    IMPLICIT NONE
    INTEGER, INTENT(IN) :: nval
    size_calls = size_calls + 1
    size_maybe_elided = nval
  END FUNCTION size_maybe_elided

  INTEGER FUNCTION sidefx_inc(x)
    IMPLICIT NONE
    INTEGER, INTENT(IN) :: x
    sidefx_calls = sidefx_calls + 1
    sidefx_inc = x
  END FUNCTION sidefx_inc

END MODULE acc_expr_eval_helpers


#ifndef T1
!T1:syntax,expressions,runtime,construct-independent,V:3.4-
LOGICAL FUNCTION test1()
  USE OPENACC
  USE acc_expr_eval_helpers
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"

  INTEGER :: i
  INTEGER :: cond
  INTEGER :: errors
  REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)

  DO i = 1, LOOPCOUNT
    c(i) = 0.0D0
  END DO

  cond = 0
  IF (LOOPCOUNT .GT. 0) THEN
    cond = 1
  ELSE
    cond = 0
  END IF

  !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
    !$acc parallel &
    !$acc& if((cond .EQ. 1) .AND. ((LOOPCOUNT/2) .GT. 0)) &
    !$acc& num_gangs(foo_pure(LOOPCOUNT + 7)) &
    !$acc& async(MOD(LOOPCOUNT, 3) + 1)

      !$acc loop
      DO i = 1, LOOPCOUNT
        c(i) = a(i) + b(i)
      END DO

    !$acc end parallel
    !$acc wait
  !$acc end data

  DO i = 1, LOOPCOUNT
    IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) THEN
      errors = errors + 1
    ELSE
      errors = errors + 0
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
!T2:syntax,expressions,runtime,construct-independent,V:3.4-
LOGICAL FUNCTION test2()
  USE OPENACC
  USE acc_expr_eval_helpers
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"

  INTEGER :: i
  INTEGER :: errors
  REAL(8), DIMENSION(LOOPCOUNT) :: a

  errors = 0

  DO i = 1, LOOPCOUNT
    a(i) = DBLE(i)
  END DO

  ! Ensure 'a' is NOT present on device: do NOT enter/create any data.
  size_calls = 0
  !$acc update device(a(1:size_maybe_elided(LOOPCOUNT))) if_present

  ! User-visible behavior: no crash; host values unchanged.
  DO i = 1, LOOPCOUNT
    IF (ABS(a(i) - DBLE(i)) .GT. PRECISION) THEN
      errors = errors + 1
    ELSE
      errors = errors + 0
    END IF
  END DO

  ! DO NOT assert anything about size_calls (may be 0, 1, or more).

  IF (errors .EQ. 0) THEN
    test2 = .FALSE.
  ELSE
    test2 = .TRUE.
  END IF
END FUNCTION test2
#endif


#ifndef T3
!T3:syntax,expressions,runtime,construct-independent,V:3.4-
LOGICAL FUNCTION test3()
  USE OPENACC
  USE acc_expr_eval_helpers
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"

  INTEGER :: i
  INTEGER :: k
  INTEGER :: tmp
  INTEGER :: errors
  REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)

  DO i = 1, LOOPCOUNT
    c(i) = 0.0D0
  END DO

  sidefx_calls = 0
  tmp = 7

  !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
    !$acc parallel &
    !$acc& num_gangs(foo_pure(sidefx_inc(tmp))) &
    !$acc& async(MOD(sidefx_inc(tmp + 2), 3) + 1)

      !$acc loop
      DO k = 1, LOOPCOUNT
        c(k) = a(k) + b(k)
      END DO

    !$acc end parallel
    !$acc wait
  !$acc end data

  DO i = 1, LOOPCOUNT
    IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) THEN
      errors = errors + 1
    ELSE
      errors = errors + 0
    END IF
  END DO

  ! DO NOT assert anything about sidefx_calls.

  IF (errors .EQ. 0) THEN
    test3 = .FALSE.
  ELSE
    test3 = .TRUE.
  END IF
END FUNCTION test3
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
  ELSE
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
  ELSE
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
  ELSE
    failed = .FALSE.
  END IF
#endif

  CALL EXIT(failcode)
END PROGRAM main
