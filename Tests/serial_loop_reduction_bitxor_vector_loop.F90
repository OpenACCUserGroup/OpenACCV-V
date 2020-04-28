#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER,DIMENSION(LOOPCOUNT, 10):: a
  INTEGER,DIMENSION(10):: b, host_b
  REAL(8),DIMENSION(LOOPCOUNT, 10):: randoms
  INTEGER:: errors, x, y, temp

  errors = 0
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      a(x, y) = INT(randoms(x, y) * 10000)
    END DO
  END DO

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      host_b(y) = ieor(host_b(y), a(x, y))
    END DO
  END DO

  !$acc data copyin(a(1:LOOPCOUNT, 1:10))
    !$acc serial loop private(temp)
    DO y = 1, 10
      temp = 0
      !$acc loop vector reduction(ieor:temp)
      DO x = 1, LOOPCOUNT
        temp = ieor(temp, a(x, y))
      END DO
      b(y) = temp
    END DO
  !$acc end data

  DO y = 1, 10
    IF (b(y) .ne. host_b(y)) THEN
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
