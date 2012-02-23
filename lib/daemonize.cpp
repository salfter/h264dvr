#include "daemonize.h"

void daemonize()
{
  int pid=fork();
  if (pid<0) exit(-1); // fork() error
  if (pid>0) exit(0); // parent exits normally
  umask(0);
  int sid=setsid();
  if (sid<0)
    exit(-1);
  if ((chdir("/"))<0)
    exit(-1);
  FILE* nullin=freopen("/dev/null", "r", stdin);
  FILE* nullout=freopen("/dev/null", "w", stdout);
  FILE* nullerr=freopen("/dev/null", "w", stderr);
}

