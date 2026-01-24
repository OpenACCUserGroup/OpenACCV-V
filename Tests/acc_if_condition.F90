! acc_if_condition.F90
! Fortran conditions must be LOGICAL.

#ifndef T1
!T1:syntax,if-clause,runtime,enter-data,V:3.4-
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a
        LOGICAL :: present

        errors = 0
        DO i=1, LOOPCOUNT
          a(i) = DBLE(i)
        END DO

        !$acc enter data copyin(a(1:LOOPCOUNT)) if(.FALSE.)
        present = acc_is_present(a)
        IF (present) errors = errors + 1

        IF (present) THEN
          !$acc exit data delete(a(1:LOOPCOUNT)) if(.TRUE.)
        END IF
        test1 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T2
!T2:syntax,if-clause,runtime,enter-data,V:3.4-
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a
        LOGICAL :: present

        errors = 0
        DO i=1, LOOPCOUNT
          a(i) = DBLE(i)
        END DO

        !$acc enter data copyin(a(1:LOOPCOUNT)) if(.TRUE.)
        present = acc_is_present(a)
        IF (.NOT. present) errors = errors + 1

        !$acc exit data delete(a(1:LOOPCOUNT)) if(.TRUE.)
        test2 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T3
!T3:syntax,if-clause,runtime,exit-data,V:3.4-
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a
        LOGICAL :: present

        errors = 0
        DO i=1, LOOPCOUNT
          a(i) = DBLE(i)
        END DO

        !$acc enter data copyin(a(1:LOOPCOUNT)) if(.TRUE.)
        present = acc_is_present(a)
        IF (.NOT. present) errors = errors + 1

        !$acc exit data delete(a(1:LOOPCOUNT)) if(.FALSE.)
        present = acc_is_present(a)
        IF (.NOT. present) errors = errors + 1

        !$acc exit data delete(a(1:LOOPCOUNT)) if(.TRUE.)
        test3 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T4
!T4:syntax,if-clause,runtime,exit-data,V:3.4-
      LOGICAL FUNCTION test4()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a
        LOGICAL :: present

        errors = 0
        DO i=1, LOOPCOUNT
          a(i) = DBLE(i)
        END DO

        !$acc enter data copyin(a(1:LOOPCOUNT)) if(.TRUE.)
        present = acc_is_present(a)
        IF (.NOT. present) errors = errors + 1

        !$acc exit data delete(a(1:LOOPCOUNT)) if(.TRUE.)
        present = acc_is_present(a)
        IF (present) errors = errors + 1

        test4 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T5
!T5:syntax,if-clause,runtime,compute,V:3.4-
! logical variable condition
      LOGICAL FUNCTION test5()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        LOGICAL :: cond

        errors = 0
        cond = .TRUE.

        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT)) if(cond)
          DO i=1, LOOPCOUNT
            c(i) = 2.0D0 * a(i)
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i=1, LOOPCOUNT
          IF (ABS(c(i) - 2.0D0*a(i)) .GT. PRECISION) errors = errors + 1
        END DO

        test5 = (errors .NE. 0)
      END FUNCTION
#endif

#ifndef T6
!T6:syntax,if-clause,runtime,compute,V:3.4-
! logical expression condition
      LOGICAL FUNCTION test6()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: i, errors
        REAL(8), DIMENSION(LOOPCOUNT) :: a, c
        LOGICAL :: cond

        errors = 0
        cond = .TRUE.

        CALL RANDOM_NUMBER(a)
        c = 0.0D0

        !$acc data copyin(a(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
          !$acc parallel loop present(a(1:LOOPCOUNT), c(1:LOOPCOUNT)) &
          !$acc& if( (LOOPCOUNT .GT. 0) .AND. cond )
          DO i=1, LOOPCOUNT
            c(i) = a(i) + 1.0D0
          END DO
          !$acc end parallel loop
        !$acc end data

        DO i=1, LOOPCOUNT
          IF (ABS(c(i) - (a(i)+1.0D0)) .GT. PRECISION) errors = errors + 1
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
        DO testrun=1, NUM_TEST_CALLS
          failed = failed .OR. test1()
        END DO
        IF (failed) failcode = failcode + 2**0
#endif
#ifndef T2
        failed = .FALSE.
        DO testrun=1, NUM_TEST_CALLS
          failed = failed .OR. test2()
        END DO
        IF (failed) failcode = failcode + 2**1
#endif
#ifndef T3
        failed = .FALSE.
        DO testrun=1, NUM_TEST_CALLS
          failed = failed .OR. test3()
        END DO
        IF (failed) failcode = failcode + 2**2
#endif
#ifndef T4
        failed = .FALSE.
        DO testrun=1, NUM_TEST_CALLS
          failed = failed .OR. test4()
        END DO
        IF (failed) failcode = failcode + 2**3
#endif
#ifndef T5
        failed = .FALSE.
        DO testrun=1, NUM_TEST_CALLS
          failed = failed .OR. test5()
        END DO
        IF (failed) failcode = failcode + 2**4
#endif
#ifndef T6
        failed = .FALSE.
        DO testrun=1, NUM_TEST_CALLS
          failed = failed .OR. test6()
        END DO
        IF (failed) failcode = failcode + 2**5
#endif

        CALL EXIT(failcode)
      END PROGRAM
