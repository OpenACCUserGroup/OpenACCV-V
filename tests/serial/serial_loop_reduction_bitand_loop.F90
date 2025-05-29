#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER,DIMENSION(LOOPCOUNT, 10):: a, b, b_copy
  INTEGER,DIMENSION(10):: c, host_c
  INTEGER:: errors, x, y, z, temp
  REAL(8):: false_margin
  REAL(8),DIMENSION(LOOPCOUNT, 10, 17):: randoms

  errors = 0
  false_margin = exp(log(.5) / LOOPCOUNT)

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      b(x, y) = INT(10 * randoms(x, y, 17))
      DO z = 1, 16
        IF (randoms(x, y, z) .lt. false_margin) THEN
          a(x, y) = a(x, y) + 2**(z - 1)
        END IF
      END DO
    END DO
  END DO

  b_copy = b

  DO y = 1, 10
    temp = a(1, y)
    DO x = 1, LOOPCOUNT
      temp = iand(temp, a(x, y))
    END DO
    host_c(y) = temp
  END DO

  !$acc data copyin(a(1:LOOPCOUNT, 1:10)) copy(b(1:LOOPCOUNT, 1:10), c(1:10))
    !$acc serial loop gang private(temp)
    DO y = 1, 10
      temp = a(1, y)
      !$acc loop worker reduction(iand:temp)
      DO x = 1, LOOPCOUNT
        temp = iand(temp, a(x, y))
      END DO
      c(y) = temp
      !$acc loop worker
      DO x = 1, LOOPCOUNT
        b(x, y) = b(x, y) + c(y)
      END DO
    END DO
  !$acc end data

  DO y = 1, 10
    IF (host_c(y) .ne. c(y)) THEN
      errors = errors + 1
    END IF
    DO x = 1, LOOPCOUNT
      IF (b(x, y) .ne. b_copy(x, y) + c(y)) THEN
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
