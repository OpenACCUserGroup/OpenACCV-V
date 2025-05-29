#ifndef T1
!T1:runtime,construct-independent,internal-control-values,set,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a !Data
        REAL(8),DIMENSION(:, :),ALLOCATABLE :: host_copy
        INTEGER :: errors = 0

        CALL RANDOM_SEED
        ALLOCATE(host_copy(acc_get_num_devices(acc_get_device_type()), LOOPCOUNT))

        IF (acc_get_device_type() .ne. acc_device_none) THEN
          !host_copy(acc_get_num_devices(acc_get_device_type()), LOOPCOUNT)
          DO x = 1, acc_get_num_devices(acc_get_device_type())
            DO y = 1, LOOPCOUNT
              CALL RANDOM_NUMBER(a)
              host_copy(x, :) = a
            END DO
            call acc_set_device_num(x, acc_get_device_type())
            !$acc enter data copyin(a(1:LOOPCOUNT))
          END DO

          DO x = 1, acc_get_num_devices(acc_get_device_type())
            call acc_set_device_num(x, acc_get_device_type())
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
            call acc_set_device_num(x, acc_get_device_type())
            !$acc exit data copyout(a(1:LOOPCOUNT))
            DO y = 1, LOOPCOUNT
              IF (abs(a(y) - (host_copy(x, y) + 1)) .gt. PRECISION) THEN
                errors = errors + 1
              END IF
            END DO
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif


      PROGRAM test_acc_set_device_num
        IMPLICIT NONE
        INTEGER :: failcode, testrun
        LOGICAL :: failed
        INCLUDE "acc_testsuite.Fh"
        !Conditionally define test functions
#ifndef T1
        LOGICAL :: test1
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
        CALL EXIT (failcode)
      END PROGRAM
