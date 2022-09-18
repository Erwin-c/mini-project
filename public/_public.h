/*
 * _public.h
 *
 *  Author: Erwin
 */

#ifndef _PUBILC_H_
#define _PUBILC_H_

#include <string>
#include <vector>

char *STRCPY(char *dest, const size_t destlen, const char *src);

char *STRNCPY(char *dest, const size_t destlen, const char *src, size_t n);

int SNPRINTF(char *dest, const size_t destlen, size_t n, const char *fmt, ...);

void DeleteLChar(char *str, const char chr);

void DeleteRChar(char *str, const char chr);

void DeleteLRChar(char *str, const char chr);

bool MKDIR(const char *filename, bool bisfilename = true);

FILE *FOPEN(const char *filename, const char *mode);

void LocalTime(char *stime, const char *fmt = NULL, const int timetvl = 0);

void timetostr(const time_t ltime, char *stime, const char *fmt = NULL);

class CCmdStr {
 private:
  std::vector<std::string> m_vCmdStr;

 public:
  CCmdStr();
  CCmdStr(const std::string &buffer, const char *sepstr,
          const bool bdelspace = false);
  void SplitToCmd(const std::string &buffer, const char *sepstr,
                  const bool bdelspace = false);
  int CmdCount();
  bool GetValue(const int inum, char *value, const int ilen = 0);
  bool GetValue(const int inum, double *value);

  ~CCmdStr();
};

class CFile {
 private:
  FILE *m_fp;
  bool m_bEnBuffer;
  char m_filename[301];
  char m_filenametmp[301];

 public:
  CFile();
  bool Open(const char *filename, const char *openmode, bool bEnBuffer = true);
  bool OpenForRename(const char *filename, const char *openmode,
                     bool bEnBuffer = true);
  bool CloseAndRename();
  // int fprintf(FILE *stream, const char *format, ...) without 'FILE *stream'
  void Fprintf(const char *fmt, ...);
  bool Fgets(char *buffer, const int readsize, bool bdelcrt = false);
  void Close();
  ~CFile();
};

class CLogFile {
 private:
  FILE *m_tracefp;
  char m_filename[301];
  char m_openmode[11];
  bool m_bEnBuffer;
  bool m_bBackup;
  long m_MaxLogSize;
  // pthread_spinlock_t spin;

 public:
  CLogFile(const long MaxLogSize = 100);
  bool Open(const char *filename, const char *openmode = NULL,
            bool bBackup = true, bool bEnBuffer = false);
  bool BackupLogFile();
  bool Write(const char *fmt, ...);
  bool WriteEx(const char *fmt, ...);
  void Close();
  ~CLogFile();
};

class CSEM {
 private:
  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
  };

  int m_semid;
  short m_sem_flg;

 public:
  CSEM();
  bool init(key_t key, unsigned short value = 1, short sem_flg = SEM_UNDO);
  bool P(short sem_op = -1);
  bool V(short sem_op = 1);
  int value();
  bool destroy();
  ~CSEM();
};

#endif  // _PUBLIC_H_
