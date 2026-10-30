#ifndef T1
!T1:runtime,data,V:3.3
    LOGICAL FUNCTION test1()
        USE ISO_C_BINDING
        USE OPENACC
        IMPLICIT NONE
        INCLUDE "acc_testsuite.Fh"
        REAL,  DIMENSION(LOOPCOUNT) :: a !Data
        REAL, POINTER, DIMENSION(:) :: a_ptr
        TYPE(C_PTR) :: a_devptr, a_hostptr
        INTEGER :: err = 0

        a = 1
        CALL acc_create(a)

        a_devptr = acc_deviceptr(a)
        a_hostptr = acc_hostptr(a_devptr)
        CALL C_F_POINTER(a_hostptr, a_ptr, [LOOPCOUNT])
        IF (ANY(a /= a_ptr)) THEN
                err = err + 1
        END IF

        CALL acc_delete(a)

        IF (err .eq. 0) THEN
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
        !Conditionally define test functions
#ifndef T1
        LOGICAL :: test1
#endif
        failcode = 0
        failed = .FALSE.

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



