/*
 * idcapp.h, 此程序是数据中心项目公用函数和类的声明文件.
 *
 * Author: Erwin
 */

#ifndef IDCAPP_H_
#define IDCAPP_H_

#include "_mysql.h"
#include "_public.h"

struct st_zhobtmind {
  char obtid[11];      // 代码.
  char ddatetime[21];  // 数据时间, 精确到分钟.
  char t[11];          // 温度, 单位: 0.1 摄氏度.
  char p[11];          // 气压, 单位: 0.1 百帕.
  char u[11];          // 相对湿度, 0-100 之间的值.
  char wd[11];         // 风向, 0-360 之间的值.
  char wf[11];         // 风速: 单位 0.1m/s.
  char r[11];          // 降雨量: 0.1mm.
  char vis[11];        // 能见度: 0.1米.
};

// 全国站点分钟观测数据操作类.
class CZHOBTMIND {
 public:
  connection *m_conn;   // 数据库连接.
  CLogFile *m_logfile;  // 日志.

  sqlstatement m_stmt;  // 插入表操作的 SQL.

  char m_buffer[1024];       // 从文件中读到的一行.
  st_zhobtmind m_zhobtmind;  // 全国站点分钟观测数据结构.

  CZHOBTMIND();
  CZHOBTMIND(connection *conn, CLogFile *logfile);

  ~CZHOBTMIND();

  // 把 connection 和 CLogFile 的传进去.
  void BindConnLog(connection *conn, CLogFile *logfile);

  // 把从文件读到的一行数据拆分到 m_zhobtmind 结构体中.
  bool SplitBuffer(char *strBuffer, bool bisxml);

  // 把 m_zhobtmind 结构体中的数据插入到 T_ZHOBTMIND 表中.
  bool InsertTable();
};

#endif  // IDCAPP_H_
