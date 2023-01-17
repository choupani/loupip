
#ifndef _STDINC_H
#define _STDINC_H

#include "./precom.h"

#define FREE_AND_NULL(x, t) {			\
    if( (x) != NULL ) {				\
      free((x), (t));				\
      (x) = NULL;				\
    }						\
  };

#define FREE_AND_ALLOC(x, p, t, f) {		\
    if( (x) != NULL ) {				\
      free((x), (t));				\
      (x) = NULL;				\
    }						\
    x = malloc((p), (t), (f));			\
  };

#define MEM_ALLOC(p, t, f)          malloc((p), (t), (f))

#define MEM_REALLOC(p, l, t, f)     reallocf((p), (l), (t), (f))

#define MEM_REALLOC_U(p, l, t, f)   realloc((p), (l), (t), (f))

#define MEM_FREE(p, t)              FREE_AND_NULL((p), (t))


#endif /* _STDINC_H */
