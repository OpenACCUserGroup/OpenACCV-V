#ifndef T1
!T1:data,executable-data,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        b = 0
        c = 0

        !$acc enter data create(b(1:LOOPCOUNT))
        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc parallel present(b(1:LOOPCOUNT))
            !$acc loop 
            DO x = 1, LOOPCOUNT
              b(x) = a(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc data copyout(c(1:LOOPCOUNT))
          !$acc parallel present(b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data delete(b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - c(x)) .gt. PRECISION) THEN
            errors = errors + 1
            EXIT
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif
#ifndef T2
!T2:data,executable-data,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        b = 0
        c = 0
        !$acc enter data present_or_create(b(1:LOOPCOUNT))
        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc parallel present(b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = a(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc data copyout(c(1:LOOPCOUNT))
          !$acc parallel present(b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data delete(b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - c(x)) .gt. PRECISION) THEN
            errors = errors + 2
            EXIT
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test2 = .FALSE.
        ELSE
          test2 = .TRUE.
        END IF
      END
#endif
#ifndef T3
!T3:data,executable-data,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test3()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        INTEGER :: errors = 0

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        b = 0
        c = 0
 
        !$acc enter data pcreate(b(1:LOOPCOUNT))
        !$acc data copyin(a(1:LOOPCOUNT))
          !$acc parallel present(b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              b(x) = a(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc data copyout(c(1:LOOPCOUNT))
          !$acc parallel present(b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data delete(b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(a(x) - c(x)) .gt. PRECISION) THEN
            errors = errors + 4
            EXIT
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
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
#ifndef T3
        LOGICAL :: test3
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
#ifndef T3
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test3()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 2
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
