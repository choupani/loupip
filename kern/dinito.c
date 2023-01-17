
#include "./dinito.h"
#include "./define.h"
#include "./mbq.h"

void 
dinit_objects(void)
{
  MQ_LOCK();
  pktlst_clear();
  MQ_UNLOCK();
  
  destroy_mbq_mutex();
}
