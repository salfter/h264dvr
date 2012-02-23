#ifndef DISKWRITER_H
#define DISKWRITER_H

#include <fstream>
#include <string>
#include <syslog.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "index_type.h"

#define MIN_FREE_SPACE 3221225472 // 3 GB

using namespace std;

class DiskWriter
{
private:
  bool opened;
  bool rotate;
  string szDestDir;
  string szPrefix;
  ofstream outfile_data, outfile_index;
  struct tm last_ts;
  int lastframe_hour;
  int lastframe_minute;
  void Cleanup();
public:
  DiskWriter(string destdir, string prefix);
  bool Write(void* bfr, int len, bool keyframe);
};


#endif // DISKWRITER_H
