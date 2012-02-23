#ifndef __RECD_H
#define __RECD_H

#include <iostream>
#include <getopt.h>
#include <string>
#include <syslog.h>
#include <libgen.h>
#include "config.h"

using namespace std;

void help(char* progname);
int main (int argc, char** argv);

#endif // __RECD_H
