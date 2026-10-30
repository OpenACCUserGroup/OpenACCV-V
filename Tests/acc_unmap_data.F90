#ifndef T1
!T1:runtime,data,executable-data,construct-independent,V:3.3
    LOGICAL FUNCTION test1()
        USE ISO_C_BINDING
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL(8),DIMENSION(LOOPCOUNT) :: a, b, c !Data
        TYPE(C_PTR) :: d
        INTEGER(C_SIZE_T) :: bytes
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

        bytes = LOOPCOUNT * C_SIZEOF(c(1))
        d = acc_malloc(bytes)
        CALL acc_map_data(c, d, bytes)

        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) present(c(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
            !$acc parallel
                !$acc loop
                    DO i = 1, LOOPCOUNT
                        c(i) = a(i) + b(i)
                    END DO
                !$acc end loop
            !$acc end parallel
        !$acc end data 

        !$acc update host(c)
                    
        DO x = 1, LOOPCOUNT
            IF (ABS(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
                errors = errors + 1
            END IF
        END DO

        CALL acc_unmap_data(c)
        CALL acc_free(d)

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
