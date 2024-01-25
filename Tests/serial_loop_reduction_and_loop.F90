#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER:: errors
  REAL(8):: false_margin
  REAL(8),DIMENSION(LOOPCOUNT, 10):: randoms
  LOGICAL,DIMENSION(LOOPCOUNT, 10):: a, a_copy
  LOGICAL,DIMENSIOn(10):: has_false
  LOGICAL:: temp
  INTEGER:: x, y
  errors = 0
  false_margin = EXP(LOG(.5) / LOOPCOUNT)

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  has_false = .FALSE.
  a = .TRUE.
  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      IF (randoms(x, y) .gt. false_margin) THEN
        a(x, y) = .FALSE.
        has_false(y) = .TRUE.
      END IF
    END DO
  END DO
  a_copy = a

  !$acc data copy(a(1:LOOPCOUNT, 1:10))
    !$acc serial loop gang private(temp)
    DO y = 1, 10
      temp = .TRUE.
      !$acc loop worker reduction(.AND.:temp)
      DO x = 1, LOOPCOUNT
        temp = temp .AND. a(x, y)
      END DO
      !$acc loop worker
      DO x = 1, LOOPCOUNT
        IF (temp) THEN
          IF (a(x, y) .eqv. .TRUE.) THEN
            a(x, y) = .FALSE.
          ELSE
            a(x, y) = .TRUE.
          END IF
        END IF
      END DO
    END DO
  !$acc end data

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      IF ((has_false(y) .eqv. .TRUE.) .AND. (a(x, y) .neqv. a_copy(x, y))) THEN
        errors = errors + 1
      ELSEIF ((has_false(y) .eqv. .FALSE.) .AND. (a(x, y) .eqv. a_copy(x, y))) THEN
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
