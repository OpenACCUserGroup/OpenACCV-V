MODULE DECLARE_COPYIN_MOD
  INTEGER,DIMENSION(10):: fixed_size_array
  !$acc declare copyin(fixed_size_array)
END MODULE DECLARE_COPYIN_MOD

