#ifndef T1
!T1:serial,reduction,combined-constructs,loop,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  REAL(8),DIMENSION(LOOPCOUNT):: a, b
  REAL(8):: maxval, host_max
  INTEGER:: errors, x

  SEEDDIM(1) = 1
  #ifdef SEED
  SEEDDIM(1) = SEED
  #endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)

  errors = 0

  DO x = 1, LOOPCOUNT
    host_max = max(host_max, a(x) * b(x))
  END DO

  !$acc data copyin(a(1:LOOPCOUNT), b(1:LOOPCOUNT))
    !$acc serial loop reduction(max:maxval)
    DO x = 1, LOOPCOUNT
      maxval = max(maxval, a(x) * b(x))
    END DO
  !$acc end data

  IF (abs(host_max - maxval) .gt. PRECISION) THEN
    errors = errors + 1
  END IF

        IF (errors .eq. 0) THEN
          test1 = .FALSE.
        ELSE
          test1 = .TRUE.
        END IF
      END
#endif

      PROGRAM serial_loop_reduction_max_general
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
