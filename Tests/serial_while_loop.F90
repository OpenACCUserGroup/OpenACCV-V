#ifndef T1
!T1:reduction,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  REAL(8),DIMENSION(LOOPCOUNT, 10):: a
  REAL(8):: avg
  INTEGER:: errors, x, y

  errors = 0

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)

  !$acc data copy(a(1:LOOPCOUNT, 1:10))
    !$acc serial
      !$acc loop
      DO y = 1, 10
        avg = 0
        DO WHILE (avg .lt. 1000)
          avg = 0
          !$acc loop reduction(+:avg)
          DO x = 1, LOOPCOUNT
            a(x, y) = a(x, y) * 1.5
            avg = avg + (a(x, y) / LOOPCOUNT)
          END DO
        END DO
      END DO
    !$acc end serial
  !$acc end data

  DO y = 1, 10
    avg = 0
    DO x = 1, LOOPCOUNT
      avg = avg + (a(x, y) / LOOPCOUNT)
    END DO
    IF ((avg .lt. 1000) .or. (avg .gt. 1500)) THEN
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
