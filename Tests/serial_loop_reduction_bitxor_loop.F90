#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER,DIMENSION(LOOPCOUNT, 10):: a, b, host_b
  INTEGER,DIMENSION(10):: c, host_c
  REAL(8),DIMENSION(LOOPCOUNT, 10, 2):: randoms
  INTEGER:: errors, x, y, temp

  errors = 0
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  c = 0
  host_c = 0
  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      b(x, y) = INT(randoms(x, y, 1) * 1000)
      a(x, y) = INT(randoms(x, y, 2) * 1000)
    END DO
  END DO
  host_b = b

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      host_c(y) = ieor(host_c(y), a(x, y))
    END DO
    DO x = 1, LOOPCOUNT
      host_b(x, y) = host_b(x, y) + host_c(y)
    END DO
  END DO

  !$acc data copyin(a(1:LOOPCOUNT, 1:10)) copy(b(1:LOOPCOUNT, 1:10), c(1:10))
    !$acc serial loop gang private(temp)
    DO y = 1, 10
      temp = 0
      !$acc loop worker reduction(ieor:temp)
      DO x = 1, LOOPCOUNT
        temp = ieor(temp, a(x, y))
      END DO
      c(y) = temp
      DO x = 1, LOOPCOUNT
        b(x, y) = b(x, y) + c(y)
      END DO
    END DO
  !$acc end data

  DO y = 1, 10
    IF (c(y) .ne. host_c(y)) THEN
      errors = errors + 1
    END IF
    DO x = 1, LOOPCOUNT
      IF (host_b(x, y) .ne. b(x, y)) THEN
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
