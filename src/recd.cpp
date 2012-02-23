#include "recd.h"
#include "daemonize.h"
#include "recd_capture.h"
#include <syslog.h>

void help(char* progname)
{
  cerr << "Usage: " << progname << " options" << endl;
  cerr << "  -s|--source /dev/videoX    use /dev/videoX as recording source" << endl;
  cerr << "  -p|--prefix foo            use foo as filename prefix for recordings" << endl;
  cerr << "  -o|--output dirname        use dirname as recording directory" << endl;
  cerr << "                               (default: /mnt/store)" << endl;
  cerr << "  -c|--caption msg           insert msg after timestamp as video caption" << endl;
  cerr << "  -d|--no-daemon             don't fork into background" << endl;
  cerr << "  -h|--help                  this message" << endl;
}

int main (int argc, char** argv)
{
  int c;
  string source="";
  string caption="";
  string prefix="";
  bool no_daemonize=0;
  string outdir="/mnt/store";
  Capture objCapture;
  
  openlog(basename(argv[0]), LOG_PID, LOG_DAEMON);
  
  while (1)
  {
    struct option long_options[]=
    {
      {"source", required_argument, 0, 's'},
      {"prefix", required_argument, 0, 'p'},
      {"output", required_argument, 0, 'o'},
      {"no-daemon", no_argument, 0, 'd'},
      {"caption", required_argument, 0, 'c'},
      {"help", no_argument, 0, 'h'}
    };
    int option_index=0;
    if ((c=getopt_long(argc, argv, "s:p:o:dc:h?", long_options, &option_index))==-1)
      break;
    switch (c)
    {
      case 's':
	source=optarg;
	break;
      case 'p':
	prefix=optarg;
	break;
      case 'o':
	outdir=optarg;
	break;
      case 'd':
	no_daemonize=1;
	break;
      case 'c':
	caption=optarg;
	break;
      case 'h':
      case '?':
	help(argv[0]);
	exit(-1);
      default:
	abort();
    }
  }
  if (optind<argc)
  {
    help(argv[0]);
    exit(-1);
  }
  if (source=="")
  {
    cerr << "error: no source specified" << endl;
    help(argv[0]);
    exit(-1);
  }
  if (prefix=="")
  {
    cerr << "error: no prefix specified" << endl;
    help(argv[0]);
    exit(-1);
  }
  if (!no_daemonize)
    daemonize();
  objCapture.StartCapture(source, prefix, outdir, caption);
  return 0; // we should never get here
}