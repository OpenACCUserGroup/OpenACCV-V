#ifndef T1
!T1:,V:2.6-2.7
      LOGICAL FUNCTION test1()
  IMPLICIT NONE
  INCLUDE "acc_testsuite.Fh"
  INTEGER :: x, i_a, i_b, i_c, i_d, i_e, i_f, i_g, i_h, i_i, i_j !Iterators
  INTEGER, PARAMETER :: PRIVATE_LOOPCOUNT = 1024
  REAL(8),DIMENSION(PRIVATE_LOOPCOUNT):: a, b, c !Data
  INTEGER :: errors
  errors = 0

  !Initilization
  SEEDDIM(1) = 1
# ifdef SEED
  SEEDDIM(1) = SEED
# endif
  CALL RANDOM_SEED(PUT=SEEDDIM)

  CALL RANDOM_NUMBER(a)
  CALL RANDOM_NUMBER(b)
  c = 0

  !$acc data copyin(a(1:PRIVATE_LOOPCOUNT), b(1:PRIVATE_LOOPCOUNT)) copy(c(1:PRIVATE_LOOPCOUNT))
    !$acc serial
      !$acc loop
      DO i_a = 0, 1
        !$acc loop
        DO i_b = 0, 1
          !$acc loop
          DO i_c = 0, 1
            !$acc loop
            DO i_d = 0, 1
              !$acc loop
              DO i_e = 0, 1
                !$acc loop
                DO i_f = 0, 1
                  !$acc loop
                  DO i_g = 0, 1
                    !$acc loop
                    DO i_h = 0, 1
                      !$acc loop
                      DO i_i = 0, 1
                        !$acc loop
                        DO i_j = 0, 1
                          c(i_a*512 + i_b*256 + i_c*128 + i_d*64 + i_e*32 + i_f*16 + i_g*8 + i_h*4 + i_i*2 + i_j + 1) = &
                          a(i_a*512 + i_b*256 + i_c*128 + i_d*64 + i_e*32 + i_f*16 + i_g*8 + i_h*4 + i_i*2 + i_j + 1) + &
                          b(i_a*512 + i_b*256 + i_c*128 + i_d*64 + i_e*32 + i_f*16 + i_g*8 + i_h*4 + i_i*2 + i_j + 1)
                        END DO
                      END DO
                    END DO
                  END DO
                END DO
              END DO
            END DO
          END DO
        END DO
      END DO
    !$acc end serial
  !$acc end data

  DO x = 1, PRIVATE_LOOPCOUNT
    IF (abs(c(x) - (a(x) + b(x))) .gt. PRECISION) THEN
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
