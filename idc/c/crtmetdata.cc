/*
 * crtmetdata.cc
 *
 *  Author: Erwin
 */

#include <_public.h>
#include <unistd.h>

#include <cstring>

CLogFile logfile;

struct st_stcode {
  char provname[31];
  char obtid[11];
  char obtname[31];
  double lat;
  double lon;
  double height;
};

struct st_metdata {
  char obtid[11];
  char ddatetime[21];
  int t;
  int p;
  int u;
  int wd;
  int wf;
  int r;
  int vis;
};

std::vector<struct st_stcode> vstcode;

std::vector<struct st_metdata> vmetdata;

char strddatetime[21];

bool LoadSTCode(const char* inifile);

void CrtMetData();

bool CrtMetFile(const char* outpath, const char* datafmt);

int main(int argc, char* argv[]) {
  if (argc != 5) {
    printf("Using: ./crtmetdata inifile outpath logfile\n");
    printf(
        "Example: ../bin/crtmetdata ../ini/stcode.ini ../../tmp/metdata "
        "../../log/idc/crtmetdata.log xml,json,csv\n");

    return -1;
  }

  if (!logfile.Open(argv[3])) {
    printf("logfile.open(%s) failed!\n", argv[1]);
    return -1;
  }

  logfile.Write("crtmetdata start\n");

  // Service code
  if (!LoadSTCode(argv[1])) {
    return -1;
  }

  CrtMetData();

  if (strstr(argv[4], "xml") != 0) {
    CrtMetFile(argv[2], "xml");
  }
  if (strstr(argv[4], "json") != 0) {
    CrtMetFile(argv[2], "json");
  }
  if (strstr(argv[4], "csv") != 0) {
    CrtMetFile(argv[2], "csv");
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

  CCmdStr CmdStr;

  struct st_stcode stcode;

  while (true) {
    // Executed in Fgets()
    // memset(strBuffer, 0, sizeof(strBuffer));
    if (!File.Fgets(strBuffer, 300, true)) {
      break;
    }

    CmdStr.SplitToCmd(strBuffer, ",", true);

    if (CmdStr.CmdCount() != 6) {
      continue;
    }

    CmdStr.GetValue(0, stcode.provname, 30);
    CmdStr.GetValue(1, stcode.obtid, 10);
    CmdStr.GetValue(2, stcode.obtname, 30);
    CmdStr.GetValue(3, &stcode.lat);
    CmdStr.GetValue(4, &stcode.lon);
    CmdStr.GetValue(5, &stcode.height);

    vstcode.push_back(stcode);
  }

  for (size_t ii = 0; ii < vstcode.size(); ++ii) {
    logfile.Write(
        "provname=%s, obtid=%s, obtname=%s, lat=%.2f, lon=%.2f, height = "
        "%.2f\n ",
        vstcode[ii].provname, vstcode[ii].obtid, vstcode[ii].obtname,
        vstcode[ii].lat, vstcode[ii].lon, vstcode[ii].height);
  }

  return true;
}

void CrtMetData() {
  srand(time(NULL));

  char strddatetime[21];
  memset(strddatetime, 0, sizeof(strddatetime));
  LocalTime(strddatetime, "yyyymmddhh24miss");

  struct st_metdata stmetdata;

  for (size_t ii = 0; ii < vstcode.size(); ++ii) {
    memset(&stmetdata, 0, sizeof(struct st_metdata));

    strncpy(stmetdata.obtid, vstcode[ii].obtid, 10);
    strncpy(stmetdata.ddatetime, strddatetime, 14);
    stmetdata.t = rand() % 351;
    stmetdata.p = rand() % 265 + 10000;
    stmetdata.u = rand() % 100 + 1;
    stmetdata.wd = rand() % 360;
    stmetdata.wf = rand() % 150;
    stmetdata.r = rand() % 16;
    stmetdata.vis = rand() % 5001 + 100000;

    vmetdata.push_back(stmetdata);
  }

  return;
}

bool CrtMetFile(const char* outpath, const char* datafmt) {
  CFile File;

  char strFileName[301];
  sprintf(strFileName, "%s/MET_ZH_%s_%d.%s", outpath, strddatetime, getpid(),
          datafmt);

  if (!File.OpenForRename(strFileName, "w")) {
    logfile.Write("File.OpenForRename(%s) failed!\n", strFileName);
    return false;
  }

  if (strcmp(datafmt, "csv") == 0) {
    File.Fprintf("Obtid,Time,TEMP,P,HUM,WDR,WS,R,VIZ\n");
  }

  for (size_t ii = 0; ii < vmetdata.size(); ++ii) {
    if (strcmp(datafmt, "csv") == 0) {
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n", vmetdata[ii].obtid,
                   vmetdata[ii].ddatetime, vmetdata[ii].t / 10.0,
                   vmetdata[ii].p / 10.0, vmetdata[ii].u, vmetdata[ii].wd,
                   vmetdata[ii].wf / 10.0, vmetdata[ii].r / 10.0,
                   vmetdata[ii].vis / 10.0);
    }
  }

  File.CloseAndRename();

  logfile.Write("Create data file %s successfully, time%s, record%d!\n",
                strFileName, strddatetime, vmetdata.size());

  return true;
}
