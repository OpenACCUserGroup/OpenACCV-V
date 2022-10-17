PROGRAM acc_shutdown
        IMPLICIT NONE
        IF (_OPENACC == 201111) THEN
          PRINT*, "1.0"
        ELSE IF (_OPENACC == 201306) THEN
          PRINT*, "2.0"
        ELSE IF (_OPENACC == 201510) THEN
          PRINT*, "2.5"
        ELSE IF (_OPENACC == 201711) THEN
          PRINT*, "2.6"
        ELSE IF (_OPENACC == 201811) THEN
          PRINT*, "2.7"
        ELSE IF (_OPENACC == 201911) THEN
          PRINT*, "3.0"
        ELSE IF (_OPENACC == 202011) THEN
          PRINT*, "3.1"
        ELSE IF (_OPENACC == 202111) THEN
          PRINT*, "3.2"
        ELSE
          CALL EXIT(1)
        END IF
        CALL EXIT (0)
      END PROGRAM

