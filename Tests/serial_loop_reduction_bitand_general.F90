#ifndef T1
!T1:serial,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER,DIMENSION(LOOPCOUNT):: a
  INTEGER:: b, host_b
  REAL(8):: false_margin
  REAL(8),DIMENSION(LOOPCOUNT, 16):: randoms
  INTEGER:: x, y
  INTEGER:: errors

  errors = 0
  false_margin = exp(log(.5)/LOOPCOUNT)

  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  DO x = 1, LOOPCOUNT
    DO y = 1, 16
      IF (randoms(x, y) .lt. false_margin) THEN
        a(x) = a(x) + 2**(y - 1)
      END IF
    END DO
  END DO

  b = a(1)
  host_b = a(1)
  DO x = 2, LOOPCOUNT
    host_b = iand(host_b, a(x))
  END DO

  !$acc data copyin(a(1:LOOPCOUNT))
    !$acc serial loop reduction(iand: b)
    DO x = 1, LOOPCOUNT
      b = iand(b, a(x))
    END DO
  !$acc end data

  IF (b .ne. host_b) THEN
    errors = errors + 1
  END IF

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
