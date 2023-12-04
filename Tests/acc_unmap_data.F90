#ifndef T1
!T1:runtime,data,executable-data,construct-independent,V:3.3
    LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8),DIMENSION(LOOPCOUNT) :: a, b, c, d, e !Data
        INTEGER :: errors = 0
        INTEGER :: x,i

        !Initilization 
        SEEDDIM(1) = 1
        #ifdef SEED
            SEEDDIM(1) = SEED
        #endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)

        CALL acc_map_data(C_LOC(c), C_LOC(d), LOOPCOUNT)

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) present(c(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
            !$acc parallel
                !$acc loop
                    DO i = 0, LOOPCOUNT
                        c(x) = a(x) + b(x)
                    END DO
                !$acc end loop
            !$acc end parallel
        !$acc end data 

        !$acc update host(c(0:n))
                    
        DO x = 0, LOOPCOUNT
            IF (ABS(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
                errors = errors + 1
            END IF
        END DO

        CALL acc_unmap_data(C_LOC(c))
        CALL acc_free(C_LOC(d))

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
