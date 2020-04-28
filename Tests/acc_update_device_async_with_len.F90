#ifndef T1
!T1:runtime,data,executable-data,async,construct-independent,update,V:2.5-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, a_host, b_host !Data
        INTEGER :: errors
        errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        a_host = a
        b_host = b

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
          CALL acc_update_device_async(a(1), LOOPCOUNT*8, 1)
          !$acc parallel async(1)
            !$acc loop
            DO x = 1, LOOPCOUNT
              a(x) = a(x) * a(x)
            END DO
          !$acc end parallel
          CALL acc_update_device_async(b(1), LOOPCOUNT*8, 2)
          !$acc parallel async(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = b(x) * b(x)
            END DO
          !$acc end parallel
          !$acc parallel async(1) wait(2)
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
          !$acc wait(1)
        !$acc end data

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - ((a_host(x) * a_host(x)) + (b_host(x) * b_host(x)))) .gt. PRECISION) THEN
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
#ifndef T2
!T2:runtime,data,executable-data,devonly,async,construct-independent,update,V:2.5-2.7
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER,DIMENSION(1):: devtest
        INTEGER :: errors
        errors = 0

        devtest(1) = 1
        !$acc enter data copyin(devtest(1:1))
        !$acc parallel present(devtest(1:1))
          devtest(1) = 0
        !$acc end parallel
        IF (devtest(1) .eq. 1) THEN
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0

          !$acc data copyout(c(1:LOOPCOUNT)) copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc parallel async(1)
              !$acc loop
              DO x = 1, LOOPCOUNT
                a(x) = a(x) * a(x)
              END DO
            !$acc end parallel
            CALL acc_update_device_async(a(1), LOOPCOUNT*8, 1)
            !$acc parallel async(2)
              !$acc loop
              DO x = 1, LOOPCOUNT
                b(x) = b(x) * b(x)
              END DO
            !$acc end parallel
            CALL acc_update_device_async(b(1), LOOPCOUNT*8, 2)
            !$acc parallel async(1), wait(2)
              !$acc loop
               DO x = 1, LOOPCOUNT
                 c(x) = a(x) + b(x)
               END DO
            !$acc end parallel
            !$acc wait(1)
          !$acc end data

          DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF
      END
#endif


      PROGRAM main
        IMPLICIT NONE
        INTEGER :: failcode, testrun
        LOGICAL :: failed
        INCLUDE "acc_testsuite.Fh"
        !Conditionally define test functions
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
        CALL EXIT (failcode)
      END PROGRAM

