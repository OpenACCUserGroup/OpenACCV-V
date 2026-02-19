! acc_integral_constant_expression.F90
!
! Feature under test (OpenACC 3.4, Section 1.6, Feb 2026):
! - Clause arguments that require an integral-constant-expression accept
!   Fortran integer constant expressions declared with PARAMETER.
!
! Notes:
! - T1: collapse() uses a PARAMETER ICE.
! - T2: tile() uses a PARAMETER ICE.
! - T3: tile() uses PARAMETER ICE values in tile( , ).
! - T4: cache() slice bounds use PARAMETER ICE values.
! - T5: cache() slice bounds use PARAMETER ICE values.
! - T6: gang(dim:) uses a PARAMETER ICE (must be 1..3).
!   Some compilers may not support gang(dim:) yet; keep for spec coverage.
!

#ifndef T1
!T1:syntax,collapse-clause,runtime,loop,V:3.4-
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER, PARAMETER :: COLL_T2 = 1 + 1
        INTEGER, PARAMETER :: M_T2 = 48, N_T2 = 12
        INTEGER :: i, j, idx, errors
        REAL(8), DIMENSION(M_T2*N_T2) :: a, b, c
        errors = 0
        DO idx = 1, M_T2*N_T2
          a(idx) = DBLE(idx+2)
          b(idx) = DBLE(idx-1)
          c(idx) = 0.0D0
        END DO
        !$acc data copyin(a(1:M_T2*N_T2), b(1:M_T2*N_T2)) copy(c(1:M_T2*N_T2))
          !$acc parallel loop collapse(COLL_T2)
          DO i = 1, M_T2
            DO j = 1, N_T2
              idx = (i-1)*N_T2 + j
              c(idx) = a(idx) + b(idx)
            END DO
          END DO
          !$acc end parallel loop
        !$acc end data
        DO idx = 1, M_T2*N_T2
          IF (ABS(c(idx)-(a(idx)+b(idx))) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO
        test1 = (errors .NE. 0)
      END FUNCTION
#endif
#ifndef T2
!T2:syntax,tile-clause,runtime,loop,V:3.4-
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER, PARAMETER :: TILE_T2 = 1 + 1
        INTEGER, PARAMETER :: M_T3 = 256
        INTEGER :: i, errors
        REAL(8), DIMENSION(M_T3) :: a, c
        errors = 0
        DO i = 1, M_T3
          a(i) = DBLE(i)
          c(i) = 0.0D0
        END DO
        !$acc data copyin(a(1:M_T3)) copy(c(1:M_T3))
          !$acc parallel loop tile(TILE_T2)
          DO i = 1, M_T3
            c(i) = 2.0D0 * a(i)
          END DO
          !$acc end parallel loop
        !$acc end data
        DO i = 1, M_T3
          IF (ABS(c(i) - 2.0D0*a(i)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO
        test2 = (errors .NE. 0)
      END FUNCTION
#endif
#ifndef T3
!T3:syntax,tile-clause,runtime,loop,V:3.4-
      LOGICAL FUNCTION test3()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER, PARAMETER :: TILE2_T4 = 1 + 1
        INTEGER, PARAMETER :: M_T4 = 64, N_T4 = 40
        INTEGER :: i, j, idx, errors
        REAL(8), DIMENSION(M_T4*N_T4) :: a, b, c
        errors = 0
        DO idx = 1, M_T4*N_T4
          a(idx) = DBLE(idx+1)
          b(idx) = DBLE(3*idx)
          c(idx) = 0.0D0
        END DO
        !$acc data copyin(a(1:M_T4*N_T4), b(1:M_T4*N_T4)) copy(c(1:M_T4*N_T4))
          !$acc parallel loop tile(TILE2_T4, TILE2_T4)
          DO i = 1, M_T4
            DO j = 1, N_T4
              idx = (i-1)*N_T4 + j
              c(idx) = a(idx) + b(idx)
            END DO
          END DO
          !$acc end parallel loop
        !$acc end data
        DO idx = 1, M_T4*N_T4
          IF (ABS(c(idx)-(a(idx)+b(idx))) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO
        test3 = (errors .NE. 0)
      END FUNCTION
#endif
#ifndef T4
!T4:syntax,cache-directive,runtime,compute,V:3.4-
      LOGICAL FUNCTION test4()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER, PARAMETER :: M_T5 = 1024
        INTEGER, PARAMETER :: L_T5 = 16
        INTEGER, PARAMETER :: LO_T5 = 32
        INTEGER :: i, errors
        REAL(8), DIMENSION(M_T5) :: p, cp
        errors = 0
        DO i = 1, M_T5
          p(i)  = DBLE(i)
          cp(i) = 0.0D0
        END DO
        !$acc data copyin(p(1:M_T5)) copy(cp(1:M_T5))

          !$acc parallel loop
          DO i = 1, M_T5
            !$acc cache(p(LO_T5:LO_T5+L_T5-1))
            cp(i) = p(i) + 1.0D0
          END DO
          !$acc end parallel loop
          

        !$acc end data
        DO i = 1, M_T5
          IF (ABS(cp(i) - (p(i)+1.0D0)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO
        test4 = (errors .NE. 0)
      END FUNCTION
#endif
#ifndef T5
!T5:syntax,cache-directive,runtime,compute,V:3.4-
      LOGICAL FUNCTION test5()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER, PARAMETER :: M_T6 = 1024
        INTEGER, PARAMETER :: LO_T6 = 40
        INTEGER, PARAMETER :: LEN_T6 = 8
        INTEGER, PARAMETER :: HI_T6 = LO_T6 + LEN_T6 - 1
        INTEGER :: i, errors
        REAL(8), DIMENSION(M_T6) :: q, cq
        errors = 0
        DO i = 1, M_T6
          q(i)  = DBLE(i)
          cq(i) = 0.0D0
        END DO
        !$acc data copyin(q(1:M_T6)) copy(cq(1:M_T6))

          !$acc parallel loop
          DO i = 1, M_T6
            !$acc cache(q(LO_T6:HI_T6))
            cq(i) = q(i) * 2.0D0
          END DO
          !$acc end parallel loop

        !$acc end data
        DO i = 1, M_T6
          IF (ABS(cq(i) - 2.0D0*q(i)) .GT. PRECISION) THEN 
            errors = errors + 1
          END IF
        END DO
        test5 = (errors .NE. 0)
      END FUNCTION
#endif
#ifndef T6
!T6:syntax,gang-clause,runtime,loop,V:3.4-
! gang(dim:DIM_T7) where dim is an integral constant expression (PARAMETER), must evaluate to 1..3
! NOTE: Some compilers may not support the 'dim:' keyword form yet; keep this as spec conformance coverage.
      LOGICAL FUNCTION test6()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER, PARAMETER :: M_T7 = 512
        INTEGER, PARAMETER :: DIM_T7 = 2
        INTEGER :: i, errors
        REAL(8), DIMENSION(M_T7) :: a, c
        errors = 0
        DO i = 1, M_T7
          a(i) = DBLE(i)
          c(i) = 0.0D0
        END DO
        !$acc data copyin(a(1:M_T7)) copy(c(1:M_T7))
          !$acc parallel loop gang(dim:DIM_T7)
          DO i = 1, M_T7
            c(i) = 2.0D0 * a(i)
          END DO
          !$acc end parallel loop
        !$acc end data
        DO i = 1, M_T7
          IF (ABS(c(i) - 2.0D0*a(i)) .GT. PRECISION) THEN
            errors = errors + 1
          END IF
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
        IF (failed) THEN
            failcode = failcode + 2**0
        END IF
#endif
#ifndef T2
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test2()
        END DO
        IF (failed) THEN
            failcode = failcode + 2**1
        END IF
#endif
#ifndef T3
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test3()
        END DO
        IF (failed) THEN
            failcode = failcode + 2**2
        END IF
#endif
#ifndef T4
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test4()
        END DO
        IF (failed) THEN
            failcode = failcode + 2**3
        END IF
#endif
#ifndef T5
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test5()
        END DO
        IF (failed) THEN
            failcode = failcode + 2**4
        END IF
#endif
#ifndef T6
        failed = .FALSE.
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .OR. test6()
        END DO
        IF (failed) THEN
            failcode = failcode + 2**5
        END IF
#endif
        CALL EXIT(failcode)
      END PROGRAM
