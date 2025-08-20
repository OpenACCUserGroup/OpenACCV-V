#define ATOMIC_CLAUSE capture
#define ATOMIC_INTEGER
#define ATOMIC_OPTYPE IAND_X_EXPR
#define ATOMIC_ASSIGN_LATER
#define ATOMIC_INIT -1
#define ATOMIC_SIZE 10
#ifndef T1
!T1:construct-independent,atomic,V:2.0-2.7
#include "atomic_template.Fh"
#endif
