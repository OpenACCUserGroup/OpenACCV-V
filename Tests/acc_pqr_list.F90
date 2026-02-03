! acc_pqr_list.F90
! Validates correct usage of OpenACC pqr-lists in Fortran as clarified in OpenACC 3.4 Section 1.6.
! Confirms that pqr-lists used in OpenACC directives:
!   - contain one or more items
!   - do not include trailing commas
! The tests exercise valid var-list pqr-lists in data clauses (copyin, copy)
! and valid int-expr-list pqr-lists in synchronization clauses (wait),
! using both single-item and multi-item lists.
! Correctness is verified at runtime by comparing device-computed results
! with expected host values.

#ifndef T1
!T1:syntax,pqr-list,runtime,construct-independent,V:3.4-
! int-expr-list non-empty via wait(1)
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c
        INTEGER :: errors
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
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async(1)
            !$acc loop
            DO i = 1, LOOPCOUNT
              c(i) = a(i) + b(i)
            END DO
          !$acc end parallel

          !$acc wait(1)
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test1 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T2
!T2:syntax,pqr-list,runtime,construct-independent,V:3.4-
! int-expr-list no trailing comma via wait(1,2)
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c
        INTEGER :: errors
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
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) async(1)
            !$acc loop
            DO i = 1, LOOPCOUNT
              c(i) = a(i) + b(i)
            END DO
          !$acc end parallel

          !$acc parallel present(c(1:LOOPCOUNT)) async(2)
            !$acc loop
            DO i = 1, LOOPCOUNT
              c(i) = c(i)
            END DO
          !$acc end parallel

          !$acc wait(1,2)
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test2 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T3
!T3:syntax,pqr-list,runtime,construct-independent,V:3.4-
! var-list non-empty via copyin(a(...))
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        INTEGER :: errors
        errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel present(a(1:LOOPCOUNT), c(1:LOOPCOUNT))
            !$acc loop
            DO i = 1, LOOPCOUNT
              c(i) = 2.0D0 * a(i)
            END DO
          !$acc end parallel
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (2.0D0 * a(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test3 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T4
!T4:syntax,pqr-list,runtime,construct-independent,V:3.4-
! var-list no trailing comma via copyin(a(...), b(...)) and present(a,b,c)
      LOGICAL FUNCTION test4()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c
        INTEGER :: errors
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
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
            !$acc loop
            DO i = 1, LOOPCOUNT
              c(i) = a(i) + b(i)
            END DO
          !$acc end parallel
        !$acc end data

        DO i = 1, LOOPCOUNT
          IF (ABS(c(i) - (a(i) + b(i))) .GT. PRECISION) errors = errors + 1
        END DO

        test4 = (errors .NE. 0)
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

        CALL EXIT(failcode)
      END PROGRAM
