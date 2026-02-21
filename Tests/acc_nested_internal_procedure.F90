! acc_nested_internal_procedure.F90
!
! Feature under test (OpenACC 3.4; clarified procedure-boundary analysis):
! - Clarified analysis of implicit data attributes and parallelism across
!   boundaries of procedures that can appear within other procedures,
!   including Fortran internal procedures (nested subprograms).
!
! Notes
! - T1 (internal procedure called from parallel loop): Enters an OpenACC
!   data region, then executes a parallel loop that calls a contained
!   subroutine which references host-associated variables (outer-scope
!   arrays/scalars). The contained subroutine is marked device-callable
!   via '!$acc routine seq'. Exercises implicit data analysis across an
!   internal-procedure boundary.
! - T2 (acc loop inside internal procedure): Enters an OpenACC 'parallel'
!   region and calls a contained subroutine that contains an '!$acc loop'
!   directive. Exercises loop/parallelism association and “orphaned loop”
!   handling across an internal-procedure boundary.

#ifndef T1
!T1:runtime,data,implicit-data,procedure-boundary,fortran-internal-procedure,construct-independent,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"

        INTEGER :: i
        INTEGER :: errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c
        REAL(8), DIMENSION(LOOPCOUNT) :: c_ref
        REAL(8) :: alpha

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0

        alpha = 3.0D0

        ! Reference result on host
        DO i = 1, LOOPCOUNT
          c_ref(i) = a(i) + b(i) + alpha
        END DO

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          ! Call an INTERNAL procedure from inside a parallel loop.
          ! This stresses that data/attribute analysis remains correct across
          ! the internal-procedure boundary, including host-associated scalars.
          !$acc parallel loop default(present) firstprivate(alpha)
          DO i = 1, LOOPCOUNT
            CALL do_point(i)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - c_ref(i)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .EQ. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF

        RETURN

      CONTAINS

        SUBROUTINE do_point(idx)
          IMPLICIT NONE
          INTEGER, INTENT(IN) :: idx
          ! Mark internal procedure as device-callable.
          !$acc routine seq
          c(idx) = a(idx) + b(idx) + alpha
        END SUBROUTINE do_point

      END FUNCTION
#endif

#ifndef T2
!T2:runtime,data,parallelism,procedure-boundary,fortran-internal-procedure,construct-independent,V:3.4-
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"

        INTEGER :: i
        INTEGER :: errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c
        REAL(8), DIMENSION(LOOPCOUNT) :: c_ref
        REAL(8) :: beta

        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0D0

        beta = 1.5D0

        ! Reference result on host
        DO i = 1, LOOPCOUNT
          c_ref(i) = beta * a(i) - b(i)
        END DO

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          ! Enter a parallel region, then run an internal procedure that
          ! contains the ACC LOOP directive (parallelism/loop context crossing
          ! an internal-procedure boundary).
          !$acc parallel default(present) firstprivate(beta)
            CALL do_loop()
          !$acc end parallel
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - c_ref(i)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .EQ. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF

        RETURN

      CONTAINS

        SUBROUTINE do_loop()
          IMPLICIT NONE
          INTEGER :: j
          !$acc routine seq
          !$acc loop
          DO j = 1, LOOPCOUNT
            c(j) = beta * a(j) - b(j)
          END DO
        END SUBROUTINE do_loop

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

        CALL EXIT(failcode)
      END PROGRAM
