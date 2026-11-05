#ifndef T1
!T1:runtime,data,executable-data,construct-independent,V:3.3
    LOGICAL FUNCTION test1()
        USE ISO_C_BINDING
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c  !Data
        REAL(8), POINTER, CONTIGUOUS, DIMENSION(:) :: a_ptr, b_ptr, d_ptr
        TYPE(C_PTR) :: a_devptr, b_devptr, d_devptr
        INTEGER :: errors = 0
        INTEGER :: x, i

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
            SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)

        DO i = 1, LOOPCOUNT
            c(i) = 0
        END DO

        !$acc enter data copyin(a, b) create(c)

        a_devptr = acc_deviceptr(a)
        b_devptr = acc_deviceptr(b)
        d_devptr = acc_deviceptr(c)
        CALL C_F_POINTER(a_devptr, a_ptr, [LOOPCOUNT])
        CALL C_F_POINTER(b_devptr, b_ptr, [LOOPCOUNT])
        CALL C_F_POINTER(d_devptr, d_ptr, [LOOPCOUNT])

        CALL device_add(a_ptr, b_ptr, d_ptr, LOOPCOUNT)

        !$acc exit data copyout(c) delete(a, b)

        DO x = 1, LOOPCOUNT
            IF (ABS(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
                errors = errors + 1
            END IF
        END DO

        IF (errors .eq. 0) THEN
            test1 = .FALSE.
        ELSE
            test1 = .TRUE.
        END IF

    CONTAINS
        ! deviceptr only accepts dummy arguments without the POINTER attribute
        ! (OpenACC 3.4 Section 2.7.5), so the device addresses come in here.
        SUBROUTINE device_add(x_a, x_b, x_d, length)
            INTEGER, INTENT(IN) :: length
            REAL(8), DIMENSION(length) :: x_a, x_b, x_d
            INTEGER :: x

            !$acc data deviceptr(x_a, x_b, x_d)
                !$acc parallel loop
                DO x = 1, length
                    x_d(x) = x_a(x) + x_b(x)
                END DO
            !$acc end data
        END SUBROUTINE device_add
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
