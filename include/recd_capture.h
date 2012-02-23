#ifndef __RECD_CAPTURE_H
#define __RECD_CAPTURE_H

#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <iostream>
#include <time.h>
#include "config.h"

#define V_BUFFERS 8
#define FRAMESIZE_WINDOW 60
#define DEAD_THRESHOLD 1024

#define reset_vbuf(__vb) do {				\
	memset((__vb), 0, sizeof(*(__vb)));		\
	(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	\
	(__vb)->memory = V4L2_MEMORY_MMAP;		\
} while(0)

using namespace std;

typedef struct {
	  void *data;
	  size_t size;
  } video_buffer;

class Capture
{
private:
  int vfd;
  struct v4l2_format vfmt;
  struct v4l2_capability vcap;
  struct v4l2_streamparm vparm;
  video_buffer p_buf[V_BUFFERS];
  void err_out(char* fmt, ...);
  void open_video_dev(const char* dev);
  void set_osd(char* fmt, ...);
  void v4l_prepare();
  string szPrevOSDMessage;
  int framesizes[FRAMESIZE_WINDOW];
  int framesize_ptr;
  string deadfilename;
public:
  void StartCapture(string source, string prefix, string outdir, string caption);
  Capture();
};

#endif // __RECD_CAPTURE_H
