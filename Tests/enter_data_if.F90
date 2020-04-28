#ifndef T1
!T1:data,executable-data,devonly,construct-independent,if,V:2.0-2.7
      LOGICAL FUNCTION test1()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b, b_copy, c !Data
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1):: devtest
        LOGICAL :: dev = .TRUE.
        LOGICAL :: cpu = .FALSE.
        devtest(1) = 1

        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev)
        !$acc data create(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev)
        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
#ifndef T2
!T2:data,executable-data,devonly,construct-independent,if,V:2.0-2.7
      LOGICAL FUNCTION test2()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b, b_copy, c !Data
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1):: devtest
        LOGICAL :: dev = .TRUE.
        LOGICAL :: cpu = .FALSE.
        devtest(1) = 1

        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        !$acc enter data create(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(cpu)
        !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data
        !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(cpu)

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
#ifndef T3
!T3:data,executable-data,devonly,construct-independent,if,V:2.0-2.7
      LOGICAL FUNCTION test3()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b, b_copy, c !Data
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1):: devtest
        LOGICAL :: dev = .TRUE.
        LOGICAL :: cpu = .FALSE.
        devtest(1) = 1

        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        IF (devtest(1) .eq. 1) THEN
          CALL RANDOM_NUMBER(a)
          a_copy = a
          CALL RANDOM_NUMBER(b)
          b_copy = b
          c = 0

          !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev)

          a = 0
          b = 0

          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data
          !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(dev)

          DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (a_copy(x) + b_copy(x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
      END
#endif
#ifndef T4
!T4:data,executable-data,devonly,construct-independent,if,V:2.0-2.7
      LOGICAL FUNCTION test4()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b, b_copy, c !Data
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1):: devtest
        LOGICAL :: dev = .TRUE.
        LOGICAL :: cpu = .FALSE.
        devtest(1) = 1

        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        IF (devtest(1) .eq. 1) THEN
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0

          !$acc enter data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(cpu)
          CALL RANDOM_NUMBER(a)
          a_copy = a
          CALL RANDOM_NUMBER(b)
          b_copy = b

          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data
          !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(cpu)

          DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (a_copy(x) + b_copy(x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test4 = .FALSE.
        ELSE
          test4 = .TRUE.
        END IF
      END
#endif
#ifndef T5
!T5:data,executable-data,devonly,construct-independent,if,V:2.0-2.7
      LOGICAL FUNCTION test5()
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, a_copy, b, b_copy, c !Data
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1):: devtest
        LOGICAL :: dev = .TRUE.
        LOGICAL :: cpu = .FALSE.
        devtest(1) = 1

        !$acc enter data copyin(devtest(1:1))
        !$acc parallel
          devtest(1) = 0
        !$acc end parallel

        !Initilization
        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)

        IF (devtest(1) .eq. 1) THEN
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0

          !$acc enter data create(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) if(cpu)
          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copyout(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data

          DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test5 = .FALSE.
        ELSE
          test5 = .TRUE.
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
#ifndef T4
        LOGICAL :: test4
#endif
#ifndef T5
        LOGICAL :: test5
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
#ifndef T4
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test4()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 3
          failed = .FALSE.
        END IF
#endif
#ifndef T5
        DO testrun = 1, NUM_TEST_CALLS
          failed = failed .or. test5()
        END DO
        IF (failed) THEN
          failcode = failcode + 2 ** 4
          failed = .FALSE.
        END IF
#endif
        CALL EXIT (failcode)
      END PROGRAM
