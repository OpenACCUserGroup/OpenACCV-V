#ifndef T1
!T1:runtime,async,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, d, a_host, b_host, c_host !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(c)
        d = 0
        a_host = a
        b_host = b
        c_host = c

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT), c(1:LOOPCOUNT)) copyout(d(1:LOOPCOUNT))
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x) = a(x) * a(x)
            END DO
          !$acc end parallel
          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = b(x) * b(x)
            END DO
          !$acc end parallel
          !$acc parallel async(3)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = c(x) * c(x)
            END DO
          !$acc end parallel
          CALL acc_wait_all()
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              d(x) = a(x) + b(x) + c(x)
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(d(x) - ((a_host(x) * a_host(x)) + (b_host(x) * b_host(x)) + (c_host(x) * c_host(x)))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

      PROGRAM main
        IMPLICIT NONE
        INTEGER :: failcode, testrun
        LOGICAL :: failed
        INCLUDE "acc_testsuite.Fh"
#ifndef T1
        LOGICAL :: test1
#endif
        failed = .FALSE.
        failcode = 0
#ifndef T1
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test1()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 0
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
