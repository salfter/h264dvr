#include "recd_capture.h"
#include "DiskWriter.h"

// capture functionality mostly cribbed from the bc-record example
// provided with the Bluecherry capture card driver

Capture::Capture()
{
  szPrevOSDMessage="";
}


void Capture::err_out(char* fmt, ... )
{
  char buf[256];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  buf[sizeof(buf) - 1] = '\0';
  
  syslog(LOG_ERR, "%s", buf);
  exit(-1);
}

void Capture::open_video_dev(const char* dev)
{
  if ((vfd = open(dev, O_RDWR)) < 0)
    err_out((char*)"Opening video device");

  /* Verify this is the correct type */
  if (ioctl(vfd, VIDIOC_QUERYCAP, &vcap) < 0)
    err_out((char*)"Querying video capabilities");

  if (!(vcap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
    !(vcap.capabilities & V4L2_CAP_STREAMING))
      err_out((char*)"Invalid video device type");

  /* Get the parameters */
  vparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(vfd, VIDIOC_G_PARM, &vparm) < 0)
    err_out((char*)"Getting parameters for video device");

  /* Get the format */
  vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(vfd, VIDIOC_G_FMT, &vfmt) < 0)
    err_out((char*)"Getting video device format");

  /* Set FPS/GOP */
  vparm.parm.capture.timeperframe.denominator = 30;
  vparm.parm.capture.timeperframe.numerator = 1;
  ioctl(vfd, VIDIOC_S_PARM, &vparm);

  /* Set format */
  vfmt.fmt.pix.width = 704;
  vfmt.fmt.pix.height = 480;
  if (ioctl(vfd, VIDIOC_S_FMT, &vfmt) < 0)
    err_out((char*)"Setting video format");
}

void Capture::set_osd(char* fmt, ...)
{
  char buf[256];
  va_list ap;
  struct v4l2_ext_control ctrl;
  struct v4l2_ext_controls ctrls;

  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  buf[sizeof(buf) - 1] = '\0';

  memset(&ctrl, 0, sizeof(ctrl));
  memset(&ctrls, 0, sizeof(ctrls));

  ctrls.count = 1;
  ctrls.ctrl_class = V4L2_CTRL_CLASS_FM_TX;
  ctrls.controls = &ctrl;
  ctrl.id = V4L2_CID_RDS_TX_RADIO_TEXT;
  ctrl.size = strlen(buf);
  ctrl.string = buf;

  ioctl(vfd, VIDIOC_S_EXT_CTRLS, &ctrls);

  return;
}

void Capture::v4l_prepare()
{
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  struct v4l2_requestbuffers req;
  int i;

  reset_vbuf(&req);
  req.count = V_BUFFERS;

  if (ioctl(vfd, VIDIOC_REQBUFS, &req) < 0)
    err_out((char*)"Requesting buffers");

  if (req.count != V_BUFFERS)
    err_out((char*)"Requested buffer count mismatch");

  for (i = 0; i < V_BUFFERS; i++) {
    struct v4l2_buffer vb;

    reset_vbuf(&vb);
    vb.index = i;

    if (ioctl(vfd, VIDIOC_QUERYBUF, &vb) < 0)
	    err_out((char*)"Querying buffer");

    p_buf[i].size = vb.length;
    p_buf[i].data = mmap(NULL, vb.length, PROT_WRITE | PROT_READ, MAP_SHARED, vfd, vb.m.offset);
    if (p_buf[i].data == MAP_FAILED)
      err_out((char*)"Mmap of buffer");
  }

  if (ioctl(vfd, VIDIOC_STREAMON, &type) < 0)
    err_out((char*)"Starting video stream");

  /* Queue all buffers */
  for (i = 0; i < V_BUFFERS; i++) {
    struct v4l2_buffer vb;

    reset_vbuf(&vb);
    vb.index = i;

    if (ioctl(vfd, VIDIOC_QBUF, &vb) < 0)
      err_out((char*)"Queuing buffer");
  }
}

void Capture::StartCapture(string source, string prefix, string outdir, string caption)
{
  bool got_vop=false;
  char szOSDMessage[160];
  
  DiskWriter dw(outdir, prefix);
  
  open_video_dev(source.c_str());
  v4l_prepare();
  
  for (;;) {
    struct v4l2_buffer vb;
    int ret;
    time_t tm = time(NULL);
    struct tm* tmv=localtime(&tm);

    if (caption!="")
      sprintf(szOSDMessage, "%04i-%02i-%02i %02i:%02i:%02i | %s", tmv->tm_year+1900, tmv->tm_mon+1, tmv->tm_mday, tmv->tm_hour, tmv->tm_min, tmv->tm_sec, caption.c_str());
    else
      sprintf(szOSDMessage, "%04i-%02i-%02i %02i:%02i:%02i", tmv->tm_year+1900, tmv->tm_mon+1, tmv->tm_mday, tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
    if (szPrevOSDMessage!=szOSDMessage)
    {
      set_osd(szOSDMessage);
      szPrevOSDMessage=szOSDMessage;
    }

    reset_vbuf(&vb);
    ret = ioctl(vfd, VIDIOC_DQBUF, &vb);
    if (ret < 0)  {
      fprintf(stderr, "Failure in dqbuf\n");
      ioctl(vfd, VIDIOC_QBUF, &vb);
      continue;
    }

    /* Wait for key frame */
    if (!got_vop) {
      if (!(vb.flags & V4L2_BUF_FLAG_KEYFRAME)) {
	ioctl(vfd, VIDIOC_QBUF, &vb);
	continue;
      }
      syslog(LOG_INFO, "starting capture on %s", source.c_str());
      got_vop = true;
    }

//     if (vb.flags & V4L2_BUF_FLAG_KEYFRAME)
//       cout << vb.bytesused << " keyframe" << endl;
//     else
//       cout << vb.bytesused << endl;

    if (!dw.Write(p_buf[vb.index].data, vb.bytesused, vb.flags & V4L2_BUF_FLAG_KEYFRAME))
      err_out((char*)"Unable to write to disk");

    ioctl(vfd, VIDIOC_QBUF, &vb);
  }

  return; // we should never get here
}