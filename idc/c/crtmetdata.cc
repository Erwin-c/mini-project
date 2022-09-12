/*
 * crtmetdata.cc
 *
 *  Author: Erwin
 */

#include <_public.h>

#include <vector>

CLogFile logfile;

struct st_stcode {
  char provname[31];
  char obtid[11];
  char obtname[31];
  double lat;
  double lon;
  double height;
};

std::vector<struct st_stcode> vstcode;

bool LoadSTCode(const char* inifile);

int main(int argc, char* argv[]) {
  // TBD: argc
  if (argc != 3) {
    printf("Using: ./crtmetdata inifile outpath logfile\n");
    printf(
        "Example: ../bin/crtmetdata ../ini/stcode.ini "
        "../../log/idc/crtmetdata.log\n");

    return -1;
  }

  if (!logfile.Open(argv[2])) {
    printf("logfile.open(%s) failed!\n", argv[1]);
    return -1;
  }

  logfile.Write("crtmetdata start\n");

  // Service code
  if (!LoadSTCode(argv[1])) {
    return -1;
  }

  logfile.Write("crtmetdata end\n");

  return 0;
}

bool LoadSTCode(const char* inifile) {
  CFile File;

  if (!File.Open(inifile, "r")) {
    logfile.Write("File.Open(%s) failed!\n", inifile);
    return false;
  }

  char strBuffer[301];

  while (true) {
    // Executed in Fgets()
    // memset(strBuffer, 0, sizeof(strBuffer));
    if (!File.Fgets(strBuffer, 300, true)) {
      break;
    }

    logfile.Write("=%s=\n", strBuffer);
  }

  return true;
}
