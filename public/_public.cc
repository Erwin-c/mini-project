/*
 * _public.cc
 *
 *  Author: Erwin
 */

#include "_public.h"

#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cstring>

char *STRCPY(char *dest, const size_t destlen, const char *src) {
  if (dest == NULL) {
    return NULL;
  }

  // Incorrect: 'sizeof(dest)' is always 8 in 64-bit os
  // memset(dest,0,sizeof(dest));
  memset(dest, 0, destlen);

  if (src == NULL) {
    return dest;
  }

  if (strlen(src) > destlen - 1) {
    strncpy(dest, src, destlen - 1);
  } else {
    strcpy(dest, src);
  }

  return dest;
}

char *STRNCPY(char *dest, const size_t destlen, const char *src, size_t n) {
  if (dest == NULL) {
    return NULL;
  }

  memset(dest, 0, destlen);
  if (src == NULL) {
    return dest;
  }

  if (n > destlen - 1) {
    strncpy(dest, src, destlen - 1);
  } else {
    strncpy(dest, src, n);
  }

  return dest;
}

int SNPRINTF(char *dest, const size_t destlen, size_t n, const char *fmt, ...) {
  if (dest == NULL) {
    return -1;
  }

  memset(dest, 0, destlen);
  int len = n;
  if (n > destlen) {
    len = destlen;
  }

  va_list arg;
  va_start(arg, fmt);
  int ret = vsnprintf(dest, len, fmt, arg);
  va_end(arg);

  return ret;
}

bool MKDIR(const char *filename, bool bisfilename) {
  char strPathName[301];
  int ilen = strlen(filename);

  for (int ii = 1; ii < ilen; ii++) {
    if (filename[ii] != '/') {
      continue;
    }

    STRNCPY(strPathName, sizeof(strPathName), filename, ii);

    if (access(strPathName, F_OK) == 0) {
      continue;
    }
    if (mkdir(strPathName, 0755) != 0) {
      return false;
    }
  }

  if (!bisfilename) {
    if (access(filename, F_OK) != 0) {
      if (mkdir(filename, 0755) != 0) {
        return false;
      }
    }
  }

  return true;
}

FILE *FOPEN(const char *filename, const char *mode) {
  if (!MKDIR(filename)) {
    return 0;
  }

  return fopen(filename, mode);
}

void LocalTime(char *stime, const char *fmt, const int timetvl) {
  if (stime == NULL) {
    return;
  }

  time_t timer;

  time(&timer);
  timer = timer + timetvl;

  timetostr(timer, stime, fmt);

  return;
}

void timetostr(const time_t ltime, char *stime, const char *fmt) {
  if (stime == NULL) {
    return;
  }

  strcpy(stime, "");

  struct tm sttm = *localtime(&ltime);
  // struct tm sttm; localtime_r(&ltime,&sttm);

  sttm.tm_year = sttm.tm_year + 1900;
  sttm.tm_mon++;

  if (fmt == NULL) {
    snprintf(stime, 20, "%04u-%02u-%02u %02u:%02u:%02u", sttm.tm_year,
             sttm.tm_mon, sttm.tm_mday, sttm.tm_hour, sttm.tm_min, sttm.tm_sec);
    return;
  }

  if (strcmp(fmt, "yyyy-mm-dd hh24:mi:ss") == 0) {
    snprintf(stime, 20, "%04u-%02u-%02u %02u:%02u:%02u", sttm.tm_year,
             sttm.tm_mon, sttm.tm_mday, sttm.tm_hour, sttm.tm_min, sttm.tm_sec);
    return;
  }

  if (strcmp(fmt, "yyyy-mm-dd hh24:mi") == 0) {
    snprintf(stime, 17, "%04u-%02u-%02u %02u:%02u", sttm.tm_year, sttm.tm_mon,
             sttm.tm_mday, sttm.tm_hour, sttm.tm_min);
    return;
  }

  if (strcmp(fmt, "yyyy-mm-dd hh24") == 0) {
    snprintf(stime, 14, "%04u-%02u-%02u %02u", sttm.tm_year, sttm.tm_mon,
             sttm.tm_mday, sttm.tm_hour);
    return;
  }

  if (strcmp(fmt, "yyyy-mm-dd") == 0) {
    snprintf(stime, 11, "%04u-%02u-%02u", sttm.tm_year, sttm.tm_mon,
             sttm.tm_mday);
    return;
  }

  if (strcmp(fmt, "yyyy-mm") == 0) {
    snprintf(stime, 8, "%04u-%02u", sttm.tm_year, sttm.tm_mon);
    return;
  }

  if (strcmp(fmt, "yyyymmddhh24miss") == 0) {
    snprintf(stime, 15, "%04u%02u%02u%02u%02u%02u", sttm.tm_year, sttm.tm_mon,
             sttm.tm_mday, sttm.tm_hour, sttm.tm_min, sttm.tm_sec);
    return;
  }

  if (strcmp(fmt, "yyyymmddhh24mi") == 0) {
    snprintf(stime, 13, "%04u%02u%02u%02u%02u", sttm.tm_year, sttm.tm_mon,
             sttm.tm_mday, sttm.tm_hour, sttm.tm_min);
    return;
  }

  if (strcmp(fmt, "yyyymmddhh24") == 0) {
    snprintf(stime, 11, "%04u%02u%02u%02u", sttm.tm_year, sttm.tm_mon,
             sttm.tm_mday, sttm.tm_hour);
    return;
  }

  if (strcmp(fmt, "yyyymmdd") == 0) {
    snprintf(stime, 9, "%04u%02u%02u", sttm.tm_year, sttm.tm_mon, sttm.tm_mday);
    return;
  }

  if (strcmp(fmt, "hh24miss") == 0) {
    snprintf(stime, 7, "%02u%02u%02u", sttm.tm_hour, sttm.tm_min, sttm.tm_sec);
    return;
  }

  if (strcmp(fmt, "hh24mi") == 0) {
    snprintf(stime, 5, "%02u%02u", sttm.tm_hour, sttm.tm_min);
    return;
  }

  if (strcmp(fmt, "hh24") == 0) {
    snprintf(stime, 3, "%02u", sttm.tm_hour);
    return;
  }

  if (strcmp(fmt, "mi") == 0) {
    snprintf(stime, 3, "%02u", sttm.tm_min);
    return;
  }

  return;
}

CLogFile::CLogFile(const long MaxLogSize) {
  m_tracefp = NULL;

  memset(m_filename, 0, sizeof(m_filename));
  memset(m_openmode, 0, sizeof(m_openmode));

  m_bBackup = true;
  m_bEnBuffer = false;

  m_MaxLogSize = MaxLogSize;
  if (m_MaxLogSize < 10) {
    m_MaxLogSize = 10;
  }

  // pthread_pin_init(&spin,0);
}

CLogFile::~CLogFile() {
  Close();

  // pthread_spin_destroy(&spin);
}

bool CLogFile::Open(const char *filename, const char *openmode, bool bBackup,
                    bool bEnBuffer) {
  Close();

  STRCPY(m_filename, sizeof(m_filename), filename);
  if (openmode == 0) {
    STRCPY(m_openmode, sizeof(m_openmode), "a+");
  } else {
    STRCPY(m_openmode, sizeof(m_openmode), openmode);
  }

  m_bEnBuffer = bEnBuffer;
  m_bBackup = bBackup;

  if ((m_tracefp = FOPEN(m_filename, m_openmode)) == NULL) {
    return false;
  }

  return true;
}

bool CLogFile::BackupLogFile() {
  if (m_tracefp == NULL) {
    return false;
  }

  if (!m_bBackup) {
    return true;
  }

  // fseek(m_tracefp, 0, 2);

  if (ftell(m_tracefp) > m_MaxLogSize * 1024 * 1024) {
    fclose(m_tracefp);
    m_tracefp = NULL;

    char strLocalTime[21];
    memset(strLocalTime, 0, sizeof(strLocalTime));
    LocalTime(strLocalTime, "yyyymmddhh24miss");

    char bak_filename[301];
    SNPRINTF(bak_filename, sizeof(bak_filename), 300, "%s.%s", m_filename,
             strLocalTime);
    rename(m_filename, bak_filename);

    if ((m_tracefp = FOPEN(m_filename, m_openmode)) == NULL) {
      return false;
    }
  }

  return true;
}

bool CLogFile::Write(const char *fmt, ...) {
  if (m_tracefp == NULL) {
    return false;
  }

  // pthread_spin_lock(&spin);

  if (!BackupLogFile()) {
    return false;
  }

  char strtime[20];
  LocalTime(strtime);

  va_list ap;
  va_start(ap, fmt);
  fprintf(m_tracefp, "%s ", strtime);
  vfprintf(m_tracefp, fmt, ap);
  va_end(ap);

  if (!m_bEnBuffer) {
    fflush(m_tracefp);
  }

  // pthread_spin_unlock(&spin);

  return true;
}

bool CLogFile::WriteEx(const char *fmt, ...) {
  if (m_tracefp == NULL) {
    return false;
  }

  // pthread_spin_lock(&spin);

  va_list ap;
  va_start(ap, fmt);
  vfprintf(m_tracefp, fmt, ap);
  va_end(ap);

  if (!m_bEnBuffer) {
    fflush(m_tracefp);
  }

  // pthread_spin_unlock(&spin);

  return true;
}

void CLogFile::Close() {
  if (m_tracefp != NULL) {
    fclose(m_tracefp);
    m_tracefp = NULL;
  }

  memset(m_filename, 0, sizeof(m_filename));
  memset(m_openmode, 0, sizeof(m_openmode));

  m_bBackup = true;
  m_bEnBuffer = false;

  return;
}
