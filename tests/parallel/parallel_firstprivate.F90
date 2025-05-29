#ifndef T1
!T1:parallel,firstprivate,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10 * LOOPCOUNT):: a, b, d !Data
        REAL(8),DIMENSION(LOOPCOUNT):: c, c_copy
        REAL*8 :: RAND
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        CALL RANDOM_NUMBER(c)
        c_copy = c
        d = 0
        
        !$acc data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT)) copy(d(1:10*LOOPCOUNT))
          !$acc parallel firstprivate(c(1:LOOPCOUNT))
            !$acc loop gang
            DO x = 0, 9
              !$acc loop worker
              DO y = 1, LOOPCOUNT
                d(x * LOOPCOUNT + y) = a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y) + c(y)
              END DO
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 0, 9
          DO y = 1, LOOPCOUNT
            IF (abs(d(x * LOOPCOUNT + y) - (a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y) + c(y))) .gt. PRECISION) THEN
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
#ifndef T2
!T2:parallel,firstprivate,V:1.0-2.7
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y !Iterators
        REAL(8),DIMENSION(10 * LOOPCOUNT):: a, b, d !Data
        REAL(8),DIMENSION(LOOPCOUNT):: c, c_copy
        REAL*8 :: RAND
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0
        d = 0
        
        !$acc data copyin(a(1:10*LOOPCOUNT), b(1:10*LOOPCOUNT)) copy(d(1:10*LOOPCOUNT))
          !$acc parallel firstprivate(c(1:LOOPCOUNT))
            !$acc loop gang independent
            DO x = 0, 9
              !$acc loop worker independent
              DO y = 1, LOOPCOUNT
                c(y) = a(x * LOOPCOUNT + y) - b(x * LOOPCOUNT + y)
              END DO
              !$acc loop worker independent
              DO y = 1, LOOPCOUNT
                d(x * LOOPCOUNT + y) = a(x * LOOPCOUNT + y) + b(x * LOOPCOUNT + y) + c(y)
              END DO
            END DO
          !$acc end parallel
        !$acc end data

        DO x = 1, 10 * LOOPCOUNT
          IF (abs(d(x) - (2 * a(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO       

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
