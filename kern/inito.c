
#include "./inito.h"
#include "./mbq.h"

void 
init_objects(void)
{
  init_mbq_mutex();
  pktlst_init();
}
