#ifndef T1
!T1:runtime,construct-independent,internal-control-values,set,V:2.5-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a !Data
        REAL(8),DIMENSION(:, :),ALLOCATABLE :: host_copy
        REAL(8) :: RAND
        INTEGER :: errors = 0
        INTEGER :: temp

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        ALLOCATE(host_copy(acc_get_num_devices(acc_get_device_type()), LOOPCOUNT))

        IF (acc_get_device_type() .ne. acc_device_none) THEN
          !host_copy(acc_get_num_devices(acc_get_device_type()), LOOPCOUNT)
          DO x = 1, acc_get_num_devices(acc_get_device_type())
            CALL RANDOM_NUMBER(a)
            host_copy(x, :) = a
            !$acc set device_num(x)
            !$acc enter data copyin(a(1:LOOPCOUNT))
          END DO

          DO x = 1, acc_get_num_devices(acc_get_device_type())
            !$acc set device_num(x)
            !$acc data present(a(1:LOOPCOUNT))
              !$acc parallel
                !$acc loop
                DO y = 1, LOOPCOUNT
                  a(y) = a(y) + 1
                END DO
              !$acc end parallel
            !$acc end data
          END DO

          DO x = 1, acc_get_num_devices(acc_get_device_type())
            !$acc set device_num(x)
            !$acc exit data copyout(a(1:LOOPCOUNT))
            temp = errors
            DO y = 1, LOOPCOUNT
              IF (abs(a(y) - (host_copy(x, y) + 1)) .gt. PRECISION) THEN
                PRINT*, a(y)
                PRINT*, host_copy(x, y)
                errors = errors + 1
              END IF
            END DO
            IF (temp .ne. errors) THEN
              PRINT*, x
            END IF
          END DO
        END IF

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
