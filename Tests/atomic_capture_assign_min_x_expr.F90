#define ATOMIC_CLAUSE capture
#define ATOMIC_MIN
#define ATOMIC_OPTYPE MIN_X_EXPR
#define ATOMIC_ASSIGN_FIRST
#define ATOMIC_INIT 1.0_8
#define ATOMIC_SIZE 10
#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
#include "atomic_template.Fh"
#endif
