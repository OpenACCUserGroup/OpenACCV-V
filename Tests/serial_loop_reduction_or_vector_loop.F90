#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  LOGICAL,DIMENSION(LOOPCOUNT, 10):: a
  LOGICAL,DIMENSION(10):: b
  LOGICAL:: temp
  REAL(8):: false_margin
  REAL(8),DIMENSION(LOOPCOUNT, 10):: randoms
  INTEGER:: errors
  INTEGER:: x, y

  errors = 0
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      IF (randoms(x, y) .gt. false_margin) THEN
        a(x, y) = .TRUE.
      ELSE
        a(x, y) = .FALSE.
      END IF
    END DO
  END DO

  !$acc data copyin(a(1:LOOPCOUNT, 1:10)) copy(b(1:10))
    !$acc serial loop private(temp)
    DO y = 1, 10
      temp = .FALSE.
      !$acc loop vector reduction(.OR.:temp)
      DO x = 1, LOOPCOUNT
        temp = temp .OR. a(x, y)
      END DO
      b(y) = temp
    END DO
  !$acc end data

  DO y = 1, 10
    temp = .FALSE.
    DO x = 1, LOOPCOUNT
      temp = temp .OR. a(x, y)
    END DO
    IF (temp .neqv. b(y)) THEN
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
