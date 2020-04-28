#ifndef T1
!T1:runtime,data,executable-data,devonly,construct-independent,update,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER,DIMENSION(1):: devtest
        REAL(8) :: RAND
        INTEGER :: errors = 0

        devtest(1) = 1
        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc data copyout(c(1:LOOPCOUNT)) create(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          CALL acc_update_device(a(1), LOOPCOUNT*8)
          CALL acc_update_device(b(1), LOOPCOUNT*8)
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
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
      END
#endif
#ifndef T2
!T2:runtime,data,executable-data,devonly,construct-independent,update,V:2.0-2.7
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER,DIMENSION(1):: devtest
        REAL(8) :: RAND
        INTEGER :: errors = 0

        devtest(1) = 1
        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        IF (devtest(1) .eq. 1) THEN
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0

          !$acc data copyout(c(1:LOOPCOUNT)) copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                a(x) = a(x) * a(x)
              END DO
            !$acc end parallel
            CALL acc_update_device(a(1), LOOPCOUNT*8)
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                b(x) = b(x) * b(x)
              END DO
            !$acc end parallel
            CALL acc_update_device(b(1), LOOPCOUNT*8)
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
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
#ifndef T1
        LOGICAL :: test1
#endif
#ifndef T2
        LOGICAL :: test2
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
