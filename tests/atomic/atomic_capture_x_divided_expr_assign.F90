RECURSIVE FUNCTION IS_POSSIBLE_2(a, b, length, init) RESULT(POSSIBLE)
  INTEGER, INTENT(IN) :: length
  REAL(8), INTENT(IN) :: init
  REAL(8),DIMENSION(length), INTENT(IN) :: a
  REAL(8),DIMENSION(length), INTENT(IN) :: b
  REAL(8),DIMENSION(length - 1) :: passed_a
  REAL(8),DIMENSION(length - 1) :: passed_b
  real(8) :: holder
  LOGICAL :: POSSIBLE
  INTEGER :: x, y
  REAL(8):: mindif
  IF (length .lt. 10) THEN
    WRITE(*, *) length
  END IF
  IF (length .eq. 0) THEN
    POSSIBLE = .TRUE.
    RETURN
  END IF
  POSSIBLE = .FALSE.

  DO x = 1, length
    IF (abs(b(x) - (init / a(x))) .lt. (100 - length) * PRECISION) THEN
      DO y = 1, x - 1
        passed_a(y) = a(y)
        passed_b(y) = b(y)
      END DO
      DO y = x + 1, length
        passed_a(y - 1) = a(y)
        passed_b(y - 1) = b(y)
      END DO
      holder = b(x)
      IF (IS_POSSIBLE_2(passed_a, passed_b, length - 1, holder)) THEN
        POSSIBLE = .TRUE.
        RETURN
      END IF
    END IF
  END DO
END FUNCTION IS_POSSIBLE_2

RECURSIVE FUNCTION IS_POSSIBLE(subset, destination, length, init) RESULT(POSSIBLE)
  INTEGER, INTENT(IN) :: length
  REAL(8),DIMENSION(length), INTENT(IN) :: subset
  REAL(8), INTENT(IN) :: destination
  REAL(8), INTENT(IN) :: init
  REAL(8),ALLOCATABLE :: passed(:)
  REAL(8) :: holder
  LOGICAL :: POSSIBLE
  INTEGER :: x, y
  IF (length .gt. 0) THEN
    ALLOCATE(passed(length - 1))
  ELSE
    IF (abs(init - destination) .lt. (10 - length) * PRECISION) THEN
      POSSIBLE = .TRUE.
    ELSE
      POSSIBLE = .FALSE.
    END IF
    RETURN
  END IF
  POSSIBLE = .FALSE.
  DO x = 1, length
    DO y = 1, x - 1
      passed(y) = subset(y)
    END DO
    DO y = x + 1, length
      passed(y - 1) = subset(y)
    END DO
    holder = init / subset(x)
    IF (IS_POSSIBLE(passed, destination, length - 1, holder)) THEN
      POSSIBLE = .TRUE.
      RETURN
    END IF
  END DO
END FUNCTION IS_POSSIBLE

#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: x, y !Iterators
  REAL(8),DIMENSION(LOOPCOUNT, 10):: a !Data
  REAL(8),DIMENSION(LOOPCOUNT, 10):: b
  REAL(8),DIMENSION(LOOPCOUNT):: totals
  REAL(8),DIMENSION(10):: passed
  REAL(8),DIMENSION(10):: passed_b
  REAL(8) :: holder
  INTEGER :: errors = 0
  LOGICAL IS_POSSIBLE
  LOGICAL IS_POSSIBLE_2

  !Initilization
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)

  totals = 1

  !$acc data copyin(a(1:LOOPCOUNT, 1:10)) copy(totals(1:LOOPCOUNT)) copyout(b(1:LOOPCOUNT,1:10))
    !$acc parallel
      !$acc loop
      DO x = 1, LOOPCOUNT
        DO y = 1, 10
          !$acc atomic capture
            totals(x) = totals(x) / a(x, y)
            b(x, y) = totals(x)
          !$acc end atomic
        END DO
      END DO
    !$acc end parallel
  !$acc end data

  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      passed(y) = a(x, y)
    END DO
    holder = 1
    IF (IS_POSSIBLE(passed, totals(x), 10, holder) .eqv. .FALSE.) THEN
      errors = errors + 1
    END IF
  END DO

  DO x = 1, LOOPCOUNT
    DO y = 1, 10
      passed(y) = a(x, y)
      passed_b(y) = b(x, y)
    END DO
    holder = 1
    IF (IS_POSSIBLE_2(passed, passed_b, 10, holder) .eqv. .FALSE.) THEN
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
