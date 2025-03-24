#define ATOMIC_CLAUSE capture
#define ATOMIC_LOGICAL
#define ATOMIC_OPTYPE X_EQV_EXPR
#define ATOMIC_ASSIGN_LATER
#define ATOMIC_INIT .FALSE.
#define ATOMIC_SIZE 10
#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
#include "atomic_template.Fh"
#endif
