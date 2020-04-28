#ifndef T1
!T1:runtime,data,executable-data,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test1()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        REAL(8) :: RAND
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
        CALL acc_copyin(a(1), LOOPCOUNT*8)
        CALL acc_copyin(b(1), LOOPCOUNT*8)
        !$acc data copy(c(1:LOOPCOUNT))
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc Loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data

        CALL acc_copyout(a(1:LOOPCOUNT))
        CALL acc_copyout(b(1:LOOPCOUNT))

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
      END FUNCTION
#endif

#ifndef T2
!T2:runtime,data,executable-data,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test2()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        CALL acc_copyin(a(1), LOOPCOUNT*8)
        CALL acc_copyin(b(1), LOOPCOUNT*8)
        !$acc data copy(c(1:LOOPCOUNT))
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data

        !$acc exit data copyout(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

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
      END FUNCTION
#endif

#ifndef T3
!T3:runtime,data,executable-data,devonly,construct-independent,V:2.5-2.7
      LOGICAL FUNCTION test3()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, a_copy, b_copy !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0
        INTEGER,DIMENSION(1) :: devtest

        devtest(1) = 1
        !$acc enter data copyin(devtest(1:1))
        !$acc parallel present(devtest(1:1))
          devtest(1) = 0
        !$acc end parallel

        IF (devtest(1) .eq. 1) THEN
          SEEDDIM(1) = 1
#         ifdef SEED
          SEEDDIM(1) = SEED
#         endif
          CALL RANDOM_SEED(PUT=SEEDDIM)
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          a_copy = a
          b_copy = b
          c = 0

          CALL acc_copyin(a(1), LOOPCOUNT*8)
          CALL acc_copyin(b(1), LOOPCOUNT*8)

          a = 0
          b = 0

          !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT)) copy(c(1:LOOPCOUNT))
            !$acc parallel
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data

          CALL acc_copyout(a(1:LOOPCOUNT))
          CALL acc_copyout(b(1:LOOPCOUNT))

          DO x = 1, LOOPCOUNT
            IF (abs(a(x) - a_copy(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
            IF (abs(b(x) - b_copy(x)) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
            IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF

        IF (errors .eq. 0) THEN
          test3 = .FALSE.
        ELSE
          test3 = .TRUE.
        END IF
      END FUNCTION
#endif

#ifndef T4
!T4:runtime,compatibility-features,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test4()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        CALL acc_pcopyin(a(1), LOOPCOUNT*8)
        CALL acc_pcopyin(b(1), LOOPCOUNT*8)

        !$acc data copyout(c(1:LOOPCOUNT)) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test4 = .FALSE.
        ELSE
          test4 = .TRUE.
        END IF
      END FUNCTION
#endif

#ifndef T5
!T5:runtime,compatibility-features,construct-independent,V:2.0-2.7
      LOGICAL FUNCTION test5()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        CALL acc_present_or_copyin(a(1), LOOPCOUNT*8)
        CALL acc_present_or_copyin(b(1), LOOPCOUNT*8)

        !$acc data copyout(c(1:LOOPCOUNT)) present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc parallel
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test5 = .FALSE.
        ELSE
          test5 = .TRUE.
        END IF
      END FUNCTION
#endif

#ifndef T6
!T6:runtime,data,executable-data,construct-independent,reference-counting,V:2.5-2.7
      LOGICAL FUNCTION test6()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0

        SEEDDIM(1) = 1
#       ifdef SEED
        SEEDDIM(1) = SEED
#       endif
        CALL RANDOM_SEED(PUT=SEEDDIM)
        CALL RANDOM_NUMBER(a)
        CALL RANDOM_NUMBER(b)
        c = 0

        CALL acc_copyin(a(1), LOOPCOUNT*8)
        CALL acc_copyin(a(1), LOOPCOUNT*8)
        CALL acc_copyin(b(1), LOOPCOUNT*8)
        CALL acc_copyin(b(1), LOOPCOUNT*8)

        !$acc data copyout(c(1:LOOPCOUNT))
          !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
            !$acc loop
            DO x = 1, LOOPCOUNT
              c(x) = a(x) + b(x)
            END DO
          !$acc end parallel
        !$acc end data

        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
        !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

        DO x = 1, LOOPCOUNT
          IF (abs(c(x) + (a(x) + b(x))) .gt. PRECISION) THEN
            errors = errors + 1
          END IF
        END DO

        IF (errors .eq. 0) THEN
          test6 = .FALSE.
        ELSE
          test6 = .TRUE.
        END IF
      END
#endif

#ifndef T7
!T7:runtime,data,executable-data,construct-independent,reference-counting,devonly,V:2.5-2.7
      LOGICAL FUNCTION test7()
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        INTEGER :: x !Iterators
        REAL(8),DIMENSION(LOOPCOUNT):: a, b, c, a_copy, b_copy !Data
        REAL(8) :: RAND
        INTEGER :: errors = 0
        LOGICAL,DIMENSION(1):: devtest

        devtest(1) = .TRUE.
        !$acc enter data copyin(devtest(1:1))
        !$acc parallel present(devtest(1:1))
          devtest(1) = .FALSE.
        !$acc end parallel

        IF (devtest(1) .eqv. .TRUE.) THEN
          SEEDDIM(1) = 1
#         ifdef SEED
          SEEDDIM(1) = SEED
#         endif
          CALL RANDOM_SEED(PUT=SEEDDIM)
          CALL RANDOM_NUMBER(a)
          CALL RANDOM_NUMBER(b)
          c = 0
          a_copy = a
          b_copy = b

          CALL acc_copyin(a(1), LOOPCOUNT*8)
          CALL acc_copyin(b(1), LOOPCOUNT*8)

          a = 0
          b = 0

          CALL acc_copyin(a(1), LOOPCOUNT*8)
          CALL acc_copyin(b(1), LOOPCOUNT*8)

          !$acc data copyout(c(1:LOOPCOUNT))
            !$acc parallel present(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
              !$acc loop
              DO x = 1, LOOPCOUNT
                c(x) = a(x) + b(x)
              END DO
            !$acc end parallel
          !$acc end data

          !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
          !$acc exit data delete(a(1:LOOPCOUNT), b(1:LOOPCOUNT))

          DO x = 1, LOOPCOUNT
            IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
              errors = errors + 1
            END IF
          END DO
        END IF
        IF (errors .eq. 0) THEN
          test7 = .FALSE.
        ELSE
          test7 = .TRUE.
        END IF
      END
#endif

      PROGRAM main
      IMPLICIT NONE
      INTEGER :: failcode, testrun
      LOGICAL :: failed
      INCLUDE "acc_testsuite.Fh"
      !Conditionally define test functions
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
#ifndef T6
      LOGICAL :: test6
#endif
#ifndef T7
      LOGICAL :: test7
#endif
      failcode = 0
      failed = .FALSE.

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
#ifndef T6
      DO testrun = 1, NUM_TEST_CALLS
        failed = failed .or. test6()
      END DO
      IF (failed) THEN
        failcode = failcode + 2 ** 5
        failed = .FALSE.
      END IF
#endif
#ifndef T7
      DO testrun = 1, NUM_TEST_CALLS
        failed = failed .or. test7()
      END DO
      IF (failed) THEN
        failcode = failcode + 2 ** 6
        failed = .FALSE.
      END IF
#endif
      CALL EXIT (failcode)
      END PROGRAM

