#ifndef T1
!T1:runtime,data,executable-data,construct-independent,V:3.3
    LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c  !Data
        REAL(8), POINTER, DIMENSION(:) :: a_ptr, b_ptr, d_ptr
        INTEGER :: errors = 0
        INTEGER :: x, i

        !Initilization
        SEEDDIM(1) = 1
        #ifdef SEED
            SEEDDIM(1) = SEED
        #endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)

        DO i = 1, LOOPCOUNT
            c(i) = 0
        END DO

        !$acc enter data copyin(a(1:n), b(1:n)) create(c(1:n))

        a_ptr = acc_deviceptr(a)
        b_ptr = acc_deviceptr(b)
        d_ptr = acc_deviceptr(c)

        !$acc data deviceptr(a_ptr, b_ptr, c_ptr)
            !$acc parallel
                !$acc loop
                DO x = 0, LOOPCOUNT
                    d_ptr(x) = a_ptr(x) + b_ptr(x);
                END DO
                !$acc end loop
            !$acc end parallel
        !$acc end data

        !$acc exit data copyout(c(1:n)) delete(a(1:n), b(1:n))

        DO x = 0, LOOPCOUNT
            IF (ABS(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
