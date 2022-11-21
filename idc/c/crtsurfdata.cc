/*
 * crtsurfdata.cc, 本程序用于生成全国气象站点观测的分钟数据.
 *
 *  Author: Erwin
 */

#include "_public.h"

// 全国气象站点参数结构体.
struct st_stcode {
  char provname[31];  // 省.
  char obtid[11];     // 站号.
  char obtname[31];   // 站名.
  double lat;         // 纬度.
  double lon;         // 经度.
  double height;      // 海拔高度.
};

// 全国气象站点分钟观测数据结构.
struct st_surfdata {
  char obtid[11];      // 站点代码.
  char ddatetime[21];  // 数据时间: 格式 yyyymmddhh24miss.
  int t;               // 气温: 单位, 0.1 摄氏度.
  int p;               // 气压: 0.1 百帕.
  int u;               // 相对湿度: 0-100 之间的值.
  int wd;              // 风向: 0-360 之间的值.
  int wf;              // 风速: 单位 0.1m/s.
  int r;               // 降雨量: 0.1mm.
  int vis;             // 能见度: 0.1米.
};

CLogFile logfile;  // 日志类.

char strddatetime[21];  // 观测数据的时间.

vector<st_stcode> vstcode;  // 存放全国气象站点参数的容器.

vector<st_surfdata> vsurfdata;  // 存放全国气象站点分钟观测数据的容器

// 把站点参数文件中加载到 vstcode 容器中.
bool LoadSTCode(const char *inifile);

// 模拟生成全国气象站点分钟观测数据, 存放在 vsurfdata 容器中.
void CrtSurfData();

// 把容器 vsurfdata 中的全国气象站点分钟观测数据写入文件.
bool CrtSurfFile(const char *outpath, const char *datafmt);

int main(int argc, char *argv[]) {
  if (argc != 5) {
    // 如果参数非法, 给出帮助文档.
    printf("Using: ./crtsurfdata inifile outpath logfile datafmt\n");
    printf(
        "Example: ~/Coding/mini-project/idc/bin/crtsurfdata "
        "~/Coding/mini-project/idc/ini/stcode.ini "
        "~/Coding/mini-project/tmp/idc/surfdata "
        "~/Coding/mini-project/log/idc/crtsurfdata.log "
        "xml,json,csv\n\n");

    printf("inifile 全国气象站点参数文件名.\n");
    printf("outpath 全国气象站点数据文件存放的目录.\n");
    printf("logfile 本程序运行的日志文件名.\n");
    printf(
        "datafmt 生成数据文件的格式, 支持xml, json 和 csv 三种格式, "
        "中间用逗号分隔.\n\n");

    return -1;
  }

  // 打开程序的日志文件.
  if (!logfile.Open(argv[3], "a+", false)) {
    printf("logfile.Open(%s) failed.\n", argv[3]);
    return -1;
  }

  logfile.Write("crtsurfdata starts.\n");

  // 把站点参数文件中加载到 vstcode 容器中.
  if (!LoadSTCode(argv[1])) {
    return -1;
  }

  // 模拟生成全国气象站点分钟观测数据, 存放在 vsurfdata 容器中.
  CrtSurfData();

  // 把容器 vsurfdata 中的全国气象站点分钟观测数据写入文件.
  if (strstr(argv[4], "xml") != 0) {
    CrtSurfFile(argv[2], "xml");
  }
  if (strstr(argv[4], "json") != 0) {
    CrtSurfFile(argv[2], "json");
  }
  if (strstr(argv[4], "csv") != 0) {
    CrtSurfFile(argv[2], "csv");
  }

  logfile.WriteEx("crtsurfdata ends.\n");

  return 0;
}

bool LoadSTCode(const char *inifile) {
  CFile File;

  // 打开站点参数文件.
  if (!File.Open(inifile, "r")) {
    logfile.Write("File.Open(%s) failed.\n", inifile);
    return false;
  }

  char strBuffer[301];

  CCmdStr CmdStr;

  st_stcode stcode;

  while (true) {
    // 从站点参数文件中读取一行, 如果已读取完, 跳出循环.
    if (!File.Fgets(strBuffer, 300, true)) {
      break;
    }

    // 把读取到的一行拆分.
    CmdStr.SplitToCmd(strBuffer, ",", true);

    if (CmdStr.CmdCount() != 6) {
      // 扔掉无效的行.
      continue;
    }

    // 把站点参数的每个数据项保存到站点参数结构体中.
    memset(&stcode, 0, sizeof(st_stcode));
    CmdStr.GetValue(0, stcode.provname, 30);  // 省.
    CmdStr.GetValue(1, stcode.obtid, 10);     // 站号.
    CmdStr.GetValue(2, stcode.obtname, 30);   // 站名.
    CmdStr.GetValue(3, &stcode.lat);          // 纬度.
    CmdStr.GetValue(4, &stcode.lon);          // 经度.
    CmdStr.GetValue(5, &stcode.height);       // 海拔高度.

    // 把站点参数结构体放入站点参数容器.
    vstcode.push_back(stcode);
  }

  return true;
}

void CrtSurfData() {
  // 播随机数种子。
  srand(time(nullptr));

  // 获取当前时间, 当作观测时间.
  memset(strddatetime, 0, sizeof(strddatetime));
  LocalTime(strddatetime, "yyyymmddhh24miss");

  st_surfdata stsurfdata;

  // 遍历气象站点参数的 vstcode 容器.
  for (size_t ii = 0; ii < vstcode.size(); ++ii) {
    memset(&stsurfdata, 0, sizeof(st_surfdata));

    // 用随机数填充分钟观测数据的结构体.
    // 站点代码.
    strncpy(stsurfdata.obtid, vstcode[ii].obtid, 10);
    // 数据时间: 格式yyyymmddhh24miss.
    strncpy(stsurfdata.ddatetime, strddatetime, 14);
    stsurfdata.t = rand() % 351;          // 气温: 单位, 0.1摄氏度.
    stsurfdata.p = rand() % 265 + 10000;  // 气压: 0.1百帕.
    stsurfdata.u = rand() % 100 + 1;      // 相对湿度: 0-100 之间的值.
    stsurfdata.wd = rand() % 360;         // 风向: 0-360 之间的值.
    stsurfdata.wf = rand() % 150;         // 风速: 单位 0.1m/s.
    stsurfdata.r = rand() % 16;           // 降雨量: 0.1mm.
    stsurfdata.vis = rand() % 5001 + 100000;  // 能见度: 0.1米.

    // 把观测数据的结构体放入 vsurfdata 容器.
    vsurfdata.push_back(stsurfdata);
  }

  return;
}

bool CrtSurfFile(const char *outpath, const char *datafmt) {
  CFile File;

  // 拼接生成数据的文件名, 例如:
  // ~/mini-project/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
  char strFileName[301];
  sprintf(strFileName, "%s/SURF_ZH_%s_%d.%s", outpath, strddatetime, getpid(),
          datafmt);

  // 打开文件.
  if (!File.OpenForRename(strFileName, "w")) {
    logfile.Write("File.OpenForRename(%s) failed.\n", strFileName);
    return false;
  }

  if (strcmp(datafmt, "csv") == 0) {
    File.Fprintf(
        "站点代码, 数据时间, 气温, 气压, 相对湿度, 风向, 风速, 降雨量, "
        "能见度\n");
  }
  if (strcmp(datafmt, "xml") == 0) {
    File.Fprintf("<data>\n");
  }
  if (strcmp(datafmt, "json") == 0) {
    File.Fprintf("{\"data\":[\n");
  }

  // 遍历存放观测数据的 vsurfdata 容器.
  for (size_t ii = 0; ii < vsurfdata.size(); ++ii) {
    // 写入一条记录.
    if (strcmp(datafmt, "csv") == 0) {
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",
                   vsurfdata[ii].obtid, vsurfdata[ii].ddatetime,
                   vsurfdata[ii].t / 10.0, vsurfdata[ii].p / 10.0,
                   vsurfdata[ii].u, vsurfdata[ii].wd, vsurfdata[ii].wf / 10.0,
                   vsurfdata[ii].r / 10.0, vsurfdata[ii].vis / 10.0);
    }
    if (strcmp(datafmt, "xml") == 0) {
      File.Fprintf(
          "<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p>"
          "<u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/"
          ">\n",
          vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, vsurfdata[ii].t / 10.0,
          vsurfdata[ii].p / 10.0, vsurfdata[ii].u, vsurfdata[ii].wd,
          vsurfdata[ii].wf / 10.0, vsurfdata[ii].r / 10.0,
          vsurfdata[ii].vis / 10.0);
    }
    if (strcmp(datafmt, "json") == 0) {
      File.Fprintf(
          "{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%."
          "1f\","
          "\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":"
          "\"%.1f\"}",
          vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, vsurfdata[ii].t / 10.0,
          vsurfdata[ii].p / 10.0, vsurfdata[ii].u, vsurfdata[ii].wd,
          vsurfdata[ii].wf / 10.0, vsurfdata[ii].r / 10.0,
          vsurfdata[ii].vis / 10.0);

      if (ii < vsurfdata.size() - 1) {
        File.Fprintf(",\n");
      } else {
        File.Fprintf("\n");
      }
    }
  }

  if (strcmp(datafmt, "xml") == 0) {
    File.Fprintf("</data>\n");
  }
  if (strcmp(datafmt, "json") == 0) {
    File.Fprintf("]}\n");
  }

  // 关闭文件.
  File.CloseAndRename();

  logfile.Write("生成数据文件 %s 成功, 数据时间 %s, 记录数 %d.\n", strFileName,
                strddatetime, vsurfdata.size());

  return true;
}
