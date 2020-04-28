#ifndef T1
!T1:parallel,private,reduction,combined-constructs,loop,V:1.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x, y, z !Iterators
        INTEGER,DIMENSION(10*LOOPCOUNT):: a, b, b_copy !Data
        REAL(8),DIMENSION(10*LOOPCOUNT):: randoms2
        INTEGER,DIMENSION(10) :: c
        REAL(8) :: false_margin
        REAL(8),DIMENSION(160*LOOPCOUNT)::randoms
        INTEGER :: errors = 0
        INTEGER :: temp

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(randoms)
        CALL RANDOM_NUMBER(randoms2)
        b = FLOOR(10000 * randoms2)
        b_copy = b
        false_margin = exp(log(.5)/LOOPCOUNT)
        DO x = 1, 10 * LOOPCOUNT
          DO y = 1, 16
            IF (randoms((y - 1) * 10 * LOOPCOUNT + x) < false_margin) THEN
              temp = 1
              DO z = 1, y
                temp = temp * 2
              END DO
              a(x) = a(x) + temp
            END IF
          END DO
        END DO
        
        DO x = 1, 10
         c(x) = a((x - 1) * LOOPCOUNT + x)
        END DO
        
        

        !$acc data copyin(a(1:10*LOOPCOUNT)) copy(b(1:10*LOOPCOUNT), c(1:10))
          !$acc parallel loop gang private(temp)
          DO x = 1, 10
            temp = a((x - 1) * LOOPCOUNT + 1)
            !$acc loop worker reduction(iand:temp)
            DO y = 2, LOOPCOUNT
              temp = iand(temp, a((x - 1) * LOOPCOUNT + y))
            END DO
            c(x) = temp
            !$acc loop worker
            DO y = 1, LOOPCOUNT
              b((x - 1) * LOOPCOUNT + y) = b((x - 1) * LOOPCOUNT + y) + c(x)
            END DO
          END DO
        !$acc end data

       DO x = 1, 10
         temp = a((x - 1) * LOOPCOUNT + 1)
         DO y = 2, LOOPCOUNT
           temp = iand(temp, a((x - 1) * LOOPCOUNT + y))
         END DO
         IF (temp .ne. c(x)) THEN
           errors = errors + 1
         END IF
         DO y = 1, LOOPCOUNT
           IF (b((x - 1) * LOOPCOUNT + y) .ne. b_copy((x - 1) * LOOPCOUNT + y) + temp) THEN
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
