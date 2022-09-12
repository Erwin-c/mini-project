/****************************************************************************************/
/*   程序名：_public.h，此程序是开发框架公用函数和类的声明文件。 */
/*   作者：吴从周 */
/****************************************************************************************/

#ifndef _PUBILC_H_
#define _PUBILC_H_

#include <sys/time.h>

#include <cstddef>
#include <cstdio>

char *STRCPY(char *dest, const size_t destlen, const char *src);

char *STRNCPY(char *dest, const size_t destlen, const char *src, size_t n);

int SNPRINTF(char *dest, const size_t destlen, size_t n, const char *fmt, ...);

bool MKDIR(const char *filename, bool bisfilename = true);

FILE *FOPEN(const char *filename, const char *mode);

void LocalTime(char *stime, const char *fmt = NULL, const int timetvl = 0);

void timetostr(const time_t ltime, char *stime, const char *fmt = NULL);

class CLogFile {
 public:
  FILE *m_tracefp;
  char m_filename[301];
  char m_openmode[11];
  bool m_bEnBuffer;
  bool m_bBackup;
  long m_MaxLogSize;
  // pthread_spinlock_t spin;。

  CLogFile(const long MaxLogSize = 100);

  ~CLogFile();

  bool Open(const char *filename, const char *openmode = NULL,
            bool bBackup = true, bool bEnBuffer = false);

  bool BackupLogFile();

  bool Write(const char *fmt, ...);
  bool WriteEx(const char *fmt, ...);

  void Close();
};

#endif  // _PUBLIC_H_
