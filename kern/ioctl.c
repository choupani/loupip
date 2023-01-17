
#include "./ioctl.h"
#include "../com/ioctl_def.h"


extern u_int8_t z_running;

static int ioctl_start(struct ioctlST*);
static int ioctl_stop(struct ioctlST*);

struct loup_ioctl
{
  u_long     cmd;
  int (*funcname) (struct ioctlST*);
  u_int8_t   copy_out;
} loup_ioctl_list[] = {

  {IOCSTART,           ioctl_start,                   0},
  {IOCSTOP,            ioctl_stop,                    0},
  
  {0,                  NULL,                          0}
};

int
parsecmd(struct ioctlST *iost, u_long cmd, u_int8_t *copy_out)
{
  int error = ERROR_INVALID;
  int n = 0;
  u_int8_t found = 0;

  while( loup_ioctl_list[n].funcname ) {

    if( cmd == loup_ioctl_list[n].cmd ) {
      error = loup_ioctl_list[n].funcname(iost);
      *copy_out = loup_ioctl_list[n].copy_out;
      found = 1;
      break;
    }
    
    ++n;
  }

  if( !found )
    printf("Not found ioctl: %lu\n", cmd);

  return (error);
}

static int
ioctl_start(struct ioctlST *iost)
{
  int error = 0;

  if (z_running)
    error = ERROR_INVALID;
  else {
    z_running = 1;
    printf("loup kernel module started.\n");
  }

  return (error);
}

static int
ioctl_stop(struct ioctlST *iost)
{
  int error = 0;

  if (!z_running)
    error = ERROR_INVALID;
  else {
    z_running = 0;
    printf("loup kernel module stopped.\n");
  }

  return (error);
}
