
#include "./str.h"
#include "./stdinc.h"

char *
mystrdup(const char *string, struct malloc_type *type)
{
  size_t len = 0;
  char *copy = NULL;
  
  if( !string ) goto done;

  len = strlen(string) + 1;
  copy = MEM_ALLOC(len, type, M_NOWAIT);
  if( copy )
    bcopy(string, copy, len);

 done:
  return (copy);
}

char *
mystrdupn(const char *string, size_t n, struct malloc_type *type)
{
  size_t len = 0;
  char *copy = NULL;
  
  if( !string ) goto done;

  len = n + 1;
  copy = MEM_ALLOC(len, type, M_NOWAIT);
  if( copy )
    bcopy(string, copy, len);

 done:
  return (copy);
}
