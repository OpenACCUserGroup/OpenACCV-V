#ifndef T1
!T1:runtime,data,executable-data,construct-independent,V:3.3
    INTEGER FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8), DIMENSION(LOOPCOUNT) :: a, b, c  
        INTEGER :: errors = 0
        INTEGER :: x, i

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)

        CALL acc_set_device_num(0, acc_device_nvidia)
        !$acc enter data create(a(1:LOOPCOUNT))
        CALL acc_set_device_num(1, acc_device_nvidia)
        !$acc enter data create(b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
            ! a(x) = rand() / (real(8)(RAND_MAX / 10))
            b(x) = a(x)
            c(x) = 4 * a(x)
        END DO
        CALL acc_set_device_num(0, acc_device_nvidia)
        !$acc update device(a(1:LOOPCOUNT))
        CALL acc_set_device_num(1, acc_device_nvidia)
        !$acc update device(b(1:LOOPCOUNT))

        CALL acc_set_device_num(0, acc_device_nvidia)
        !$acc data present(a)
            !$acc parallel loop
            DO i = 1, LOOPCOUNT
                a(i) = a(i) * 2
            END DO
            CALL acc_memcpy_d2d(b, a, LOOPCOUNT * 8, 1, 0)
        !$acc end data

        CALL acc_set_device_num(1, acc_device_nvidia)
        !$acc parallel loop
        DO i = 1, LOOPCOUNT
            b(i) = b(i) * 2
        END DO

        !$acc update host(b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
            IF (ABS(b(x) - c(x)) > PRECISION) THEN
                errors = errors + 1
                EXIT
            END IF
        END DO

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

        test1 = errors
    END FUNCTION test1
#endif

PROGRAM main
    IMPLICIT NONE
    INTEGER :: failcode, testrun
    INTEGER :: failed
    INCLUDE "acc_testsuite.Fh"
#ifndef T1
    INTEGER :: test1
#endif
    failed = 0
    failcode = 0
#ifndef T1
    DO testrun = 1, NUM_TEST_CALLS
        failed = failed + test1()
    END DO
    IF (failed /= 0) THEN
        failcode = failcode + 2 ** 0
    END IF
#endif
    CALL EXIT (failcode)
END PROGRAM main
