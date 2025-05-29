#ifndef T1
!T1:serial,private,reduction,combined-constructs,loop,V:2.6-2.7
  LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER,DIMENSION(LOOPCOUNT, 10):: a
  INTEGER,DIMENSION(10):: b, b_host
  REAL(8),DIMENSION(LOOPCOUNT, 10, 16):: randoms
  REAL(8):: false_margin
  INTEGER:: x, y, temp, c, z
  INTEGER:: errors

  errors = 0
  false_margin = exp(log(.5) / LOOPCOUNT)
  SEEDDIM(1) = 1
  #ifdef SEED
  SEEDDIM(1) = SEED
  #endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(randoms)

  DO y = 1, 10
    DO x = 1, LOOPCOUNT
      DO z = 1, 16
        IF (randoms(x, y, z) .lt. false_margin) THEN
          a(x, y) = a(x, y) + 2**(z - 1)
        END IF
      END DO
    END DO
  END DO

  DO y = 1, 10
    b_host(y) = a(1, y)
    DO x = 1, LOOPCOUNT
      b_host(y) = iand(b_host(y), a(x, y))
    END DO
  END DO

  !$acc data copyin(a(1:LOOPCOUNT, 1:10)) copy(b(1:10))
    !$acc serial loop private(c)
    DO y = 1, 10
      c = a(1, y)
      !$acc loop vector reduction(iand:c)
      DO x = 1, LOOPCOUNT
        c = iand(c, a(x, y))
      END DO
      b(y) = c
    END DO
  !$acc end data

  DO x = 1, 10
    IF (b(x) .ne. b_host(x)) THEN
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

      PROGRAM serial_loop_reduction_bitand_vector_loop
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
