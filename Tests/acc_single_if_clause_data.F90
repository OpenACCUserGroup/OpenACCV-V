! acc_single_if_clause_data.F90
!
! Feature under test (OpenACC 3.4, Sections 2.6.5, 2.6.6, 2.8. March 2026):
! The specification clarifies that the data, enter data, exit data,
! and host_data constructs may contain at most one if clause.
! These tests verify correct behavior when a single valid if clause
! is used on these constructs in Fortran.
!
! Tests:
! T1 – data construct with single if clause:
!      Executes a data region with copyin/copyout and a single
!      if(dev) clause controlling execution.
!
! T2 – enter data / exit data with single if clause:
!      Uses enter data and exit data directives with if(dev)
!      to manage device memory and verify correct results.
!
! T3 – host_data construct with single if clause:
!      Uses host_data use_device(...) with if(dev) to ensure
!      device pointer access functions correctly.
!
! T4 – combined constructs with single if clauses:
!      Uses both data and host_data constructs within the
!      same region, each containing a single if clause.

#ifndef T1
!T1:syntax,data,if-clause,construct-independent,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        INTEGER :: dev
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

        dev = 1
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT)) if(dev .ne. 0)
          !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            c(x) = a(x) + b(x)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
!T2:syntax,enter-data,exit-data,if-clause,construct-independent,V:3.4-
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        INTEGER :: dev
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c

        dev = 1
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0.0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) create(c(1:LOOPCOUNT)) if(dev .ne. 0)

        !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT))
        DO x = 1, LOOPCOUNT
          c(x) = a(x) + b(x)
        END DO
        !$acc end parallel loop

        !$acc exit data copyout(c(1:LOOPCOUNT)) delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev .ne. 0)

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
!T3:syntax,host-data,if-clause,construct-independent,V:3.4-
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        INTEGER :: dev
        REAL(8), DIMENSION(LOOPCOUNT), TARGET :: a
        REAL(8), POINTER :: seen_ptr(:)

        dev = 1
        NULLIFY(seen_ptr)

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)

        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc host_data use_device(a) if(dev .ne. 0)
            seen_ptr => a
          !$acc end host_data
        !$acc end data

        IF (.not. ASSOCIATED(seen_ptr)) THEN
          errors = errors + 1
        END IF

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
      END FUNCTION
#endif

#ifndef T4
!T4:syntax,data,host-data,if-clause,construct-independent,V:3.4-
      LOGICAL FUNCTION test4()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x
        INTEGER :: errors = 0
        INTEGER :: host
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b

        host = 0
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        b = 0.0

        !$acc data copyin(a(1:LOOPCOUNT)) copyout(b(1:LOOPCOUNT)) if(.TRUE.)
          !$acc parallel loop present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          DO x = 1, LOOPCOUNT
            b(x) = a(x) * 2.0
          END DO
          !$acc end parallel loop

          !$acc host_data use_device(b) if(host .ne. 0)
          !$acc end host_data
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(b(x) - (a(x) * 2.0)) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test4 = .FALSE.
        ELSE
          test4 = .TRUE.
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
#ifndef T4
        LOGICAL :: test4
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
#ifndef T4
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test4()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 3
          failed = .FALSE.
        END IF
#endif

        CALL EXIT(failcode)
      END PROGRAM
