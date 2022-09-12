/*
 * crtmetdata.cc
 *
 *  Author: Erwin
 */

#include <_public.h>
#include <stdio.h>

CLogFile logfile(10);

int main(int argc, char *argv[]) {
  // TBD: argc
  if (argc != 2) {
    printf("Using: ./crtmetdata inifile outpath logfile\n");
    printf("Example: ../bin/crtmetdata ../../log/idc/crtmetdata.log\n");

    return -1;
  }

  if (!logfile.Open(argv[1])) {
    printf("logfile.open(%s) failed!\n", argv[1]);
    return -1;
  }

  logfile.Write("crtmetdata start\n");

  // Service code
  for (int ii = 0; ii < 1000000; ++ii) {
    logfile.Write("This is %d010d log record\n", ii);
  }

  logfile.Write("crtmetdata end\n");

  return 0;
}
