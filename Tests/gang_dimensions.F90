#ifndef T1
!T1:parallel,gang,dim,V:3.3
    LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        INTEGER :: y
        REAL(8), DIMENSION(SMALL_LOOPCOUNT,SMALL_LOOPCOUNT) :: a, b, c !Data
        INTEGER :: errors = 0

        !Initialization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        b = a + 1.0

        !$acc data copyin(a(1:SMALL_LOOPCOUNT,1:SMALL_LOOPCOUNT), b(1:SMALL_LOOPCOUNT,1:SMALL_LOOPCOUNT)) copy(c(1:SMALL_LOOPCOUNT,1:SMALL_LOOPCOUNT))
          !$acc parallel loop num_gangs(4,4) gang(dim:2)
          DO x = 1, SMALL_LOOPCOUNT
            !$acc loop gang(dim:1)
            DO y = 1, SMALL_LOOPCOUNT
              c(x,y) = a(x,y) + 1.0
            END DO
          END DO
        !$acc end data

        DO x = 1, SMALL_LOOPCOUNT
          DO y = 1, SMALL_LOOPCOUNT
            IF (abs(c(x,y) - (b(x,y))) .gt. PRECISION) THEN
               errors = errors + 1
            END IF
          END DO
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
