RECURSIVE FUNCTION IS_POSSIBLE(a, b, length, init) RESULT(POSSIBLE)
  INTEGER, INTENT(IN) :: length
  INTEGER, INTENT(IN) :: init
  INTEGER,DIMENSION(length), INTENT(IN) :: a
  INTEGER,DIMENSION(length), INTENT(IN) :: b
  INTEGER,DIMENSION(length - 1) :: passed_a
  INTEGER,DIMENSION(length - 1) :: passed_b
  INTEGER :: holder
  LOGICAL :: POSSIBLE
  INTEGER :: x, y

  IF (length .eq. 0) THEN
    POSSIBLE = .TRUE.
    RETURN
  END IF
  POSSIBLE = .FALSE.

  DO x = 1, length
    IF (b(x) .eq. iand(init, a(x))) THEN
      DO y = 1, x - 1
        passed_a(y) = a(y)
        passed_b(y) = b(y)
      END DO
      DO y = x + 1, length
        passed_a(y - 1) = a(y)
        passed_b(y - 1) = b(y)
      END DO
      holder = b(x)
      IF (IS_POSSIBLE(passed_a, passed_b, length - 1, holder)) THEN
        POSSIBLE = .TRUE.
        RETURN
      END IF
    END IF
  END DO
END FUNCTION IS_POSSIBLE

#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: x, y, z !Iterators
  INTEGER,DIMENSION(LOOPCOUNT, 10):: a, b !Data
  REAL(8),DIMENSION(LOOPCOUNT, 10, 8):: randoms
  INTEGER,DIMENSION(LOOPCOUNT):: totals, totals_comparison
  INTEGER,DIMENSION(10):: passed_a, passed_b
  INTEGER:: init
  LOGICAL IS_POSSIBLE
  INTEGER :: errors = 0

  !Initilization
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)
  a = 0
  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      DO z = 1, 8
        IF (randoms(x, y, z) .lt. .933) THEN
          a(x, y) = a(x, y) + ISHFT(1, z - 1)
        END IF
      END DO
    END DO
  END DO
  totals = 0
  totals_comparison = 0
  DO x = 1, LOOPCOUNT
    DO y = 0, 7
      totals(x) = totals(x) + ISHFT(1, y)
      totals_comparison(x) = totals_comparison(x) + ISHFT(1, y)
    END DO
  END DO

  !$acc data copyin(a(1:LOOPCOUNT, 1:10)) copy(totals(1:LOOPCOUNT)) copyout(b(1:LOOPCOUNT, 1:10))
    !$acc parallel
      !$acc loop
      DO x = 1, LOOPCOUNT
        DO y = 1, 10
          !$acc atomic capture
            totals(x) = iand(totals(x), a(x, y))
            b(x, y) = totals(x)
          !$acc end atomic
        END DO
      END DO
    !$acc end parallel
  !$acc end data
  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      totals_comparison(x) = iand(totals_comparison(x), a(x, y))
    END DO
  END DO
  DO x = 1, LOOPCOUNT
    IF (totals_comparison(x) .NE. totals(x)) THEN
      errors = errors + 1
      WRITE(*, *) totals_comparison(x)
    END IF
  END DO

  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      passed_a(y) = a(x, y)
      passed_b(y) = b(x, y)
    END DO
    init = 0
    DO y = 0, 7
      init = init + ISHFT(1, y)
    END DO
    IF (IS_POSSIBLE(passed_a, passed_b, 10, init) .eqv. .FALSE.) THEN
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
