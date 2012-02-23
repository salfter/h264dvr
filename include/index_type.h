#ifndef __INDEX_TYPE_H
#define __INDEX_TYPE_H

#include <time.h>

typedef struct
{
  struct timespec time;
  long pos;
  int len;
  bool keyframe;
} index_t;

#endif // __INDEX_TYPE_H