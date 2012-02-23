#include "DiskWriter.h"

DiskWriter::DiskWriter(string destdir, string prefix)
{
  szDestDir=destdir;
  szPrefix=prefix;
  opened=false;
  rotate=false;
  lastframe_hour=-1;
  lastframe_minute=-1;
}

void DiskWriter::Cleanup()
{
  struct statvfs diskinfo;
  DIR* d;
  struct dirent* de;
  struct stat s;
  char pn[PATH_MAX];
  char oldest[NAME_MAX];
  time_t mtime;
  
  statvfs(szDestDir.c_str(), &diskinfo);
  if (diskinfo.f_bavail*diskinfo.f_bsize<MIN_FREE_SPACE)
  {
    time(&mtime);
    if ((d=opendir(szDestDir.c_str()))!=NULL)
    {
      while ((de=readdir(d))!=NULL)
      {
//	if (strncmp(de->d_name, szPrefix.c_str(), szPrefix.length())==0)
//	{
	  sprintf(pn, "%s/%s", szDestDir.c_str(), de->d_name);
	  if (stat(pn, &s)==0)
	    if (S_ISREG(s.st_mode) && s.st_mtime<mtime)
	    {
	      strcpy(oldest, de->d_name);
	      mtime=s.st_mtime;
	    }
//	}
      }
      closedir(d);
      sprintf(pn, "%s/%s", szDestDir.c_str(), oldest);
      unlink(pn);
    }
  }
  return;
}


bool DiskWriter::Write(void* bfr, int len, bool keyframe)
{
  struct timespec ts;
  struct tm *lt;
  char szDestFile[1024];
  index_t idx;
  
  clock_gettime(CLOCK_REALTIME, &ts);
  lt=localtime(&(ts.tv_sec));
//  if (lastframe_hour>-1 && lastframe_hour!=lt->tm_hour) 
//    rotate=true;
//  if (rotate && keyframe) // need to rotate?
//  {
//    outfile_data.close();
//    outfile_index.close();
//    opened=false;
//  }
  if (!opened)
  {    
    sprintf(szDestFile, "%s/%s.%04i%02i%02i.%02i%02i%02i%03li.264", szDestDir.c_str(), szPrefix.c_str(),
      lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday,
      lt->tm_hour, lt->tm_min, lt->tm_sec, ts.tv_nsec/1000000);
    outfile_data.open(szDestFile, ios::binary);
    outfile_index.open(((string)szDestFile+".idx").c_str(), ios::binary);
    if (outfile_data.fail())
      return false;
    opened=true;
  }
  if (lastframe_hour>-1 && lastframe_hour!=lt->tm_hour) 
    rotate=true;
  if (rotate && keyframe) // need to rotate?
  {
    outfile_data.close();
    outfile_index.close();
    sprintf(szDestFile, "%s/%s.%04i%02i%02i.%02i%02i%02i%03li.264", szDestDir.c_str(), szPrefix.c_str(),
      lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday,
      lt->tm_hour, lt->tm_min, lt->tm_sec, ts.tv_nsec/1000000);
    outfile_data.open(szDestFile, ios::binary);
    outfile_index.open(((string)szDestFile+".idx").c_str(), ios::binary);
    if (outfile_data.fail())
      return false;
    rotate=false;
  }
  if (lastframe_minute!=lt->tm_min) // check every minute for free space
    Cleanup(); 
  lastframe_hour=lt->tm_hour;
  lastframe_minute=lt->tm_min;
  memcpy(&(idx.time), &ts, sizeof(ts));
  idx.pos=outfile_data.tellp();
  idx.len=len;
  idx.keyframe=keyframe;
  outfile_data.write((const char*)bfr, len);
  outfile_index.write((const char*)&idx, sizeof(idx));
  memcpy(&last_ts, &ts, sizeof(ts));
  return !outfile_data.fail();
}
