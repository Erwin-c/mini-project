/*
 * _public.cc
 *
 *  Author: Erwin
 */

#include "_public.h"

#include <signal.h>
#include <stdarg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

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

  // TBD: Why 'destlen - 1'
  if (strlen(src) > destlen - 1) {
    // The behavior is undefined if the strings overlap
    strncpy(dest, src, destlen - 1);
  } else {
    // The behavior is undefined if the dest array is not large enough
    // The behavior is undefined if the strings overlap
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

void DeleteLChar(char *str, const char chr) {
  if (str == NULL) {
    return;
  }

  if (strlen(str) == 0) {
    return;
  }

  char strTemp[strlen(str) + 1];

  int iTemp = 0;

  memset(strTemp, 0, sizeof(strTemp));
  strcpy(strTemp, str);

  while (strTemp[iTemp] == chr) {
    ++iTemp;
  }

  memset(str, 0, strlen(str) + 1);

  strcpy(str, strTemp + iTemp);

  return;
}

void DeleteRChar(char *str, const char chr) {
  if (str == NULL) {
    return;
  }

  if (strlen(str) == 0) {
    return;
  }

  int istrlen = strlen(str);

  while (istrlen > 0) {
    if (str[istrlen - 1] != chr) {
      break;
    }

    str[istrlen - 1] = 0;

    --istrlen;
  }

  return;
}

void DeleteLRChar(char *str, const char chr) {
  DeleteLChar(str, chr);
  DeleteRChar(str, chr);

  return;
}

void PickNumber(const char *src, char *dest, const bool bsigned,
                const bool bdot) {
  if (dest == 0) {
    return;
  }
  if (src == 0) {
    strcpy(dest, "");
    return;
  }

  char strtemp[strlen(src) + 1];
  memset(strtemp, 0, sizeof(strtemp));
  strcpy(strtemp, src);
  DeleteLRChar(strtemp, ' ');

  int ipossrc, iposdst, ilen;
  ipossrc = iposdst = ilen = 0;

  ilen = strlen(strtemp);

  for (ipossrc = 0; ipossrc < ilen; ++ipossrc) {
    if (bsigned && strtemp[ipossrc] == '+') {
      dest[iposdst++] = strtemp[ipossrc];
      continue;
    }

    if (bsigned && strtemp[ipossrc] == '-') {
      dest[iposdst++] = strtemp[ipossrc];
      continue;
    }

    if (bdot && strtemp[ipossrc] == '.') {
      dest[iposdst++] = strtemp[ipossrc];
      continue;
    }

    if (isdigit(strtemp[ipossrc])) {
      dest[iposdst++] = strtemp[ipossrc];
    }
  }

  dest[iposdst] = 0;

  return;
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

bool UTime(const char *filename, const char *mtime) {
  struct utimbuf stutimbuf;

  stutimbuf.actime = stutimbuf.modtime = strtotime(mtime);

  if (utime(filename, &stutimbuf) != 0) {
    return false;
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

time_t strtotime(const char *stime) {
  char strtime[21], yyyy[5], mm[3], dd[3], hh[3], mi[3], ss[3];
  memset(strtime, 0, sizeof(strtime));
  memset(yyyy, 0, sizeof(yyyy));
  memset(mm, 0, sizeof(mm));
  memset(dd, 0, sizeof(dd));
  memset(hh, 0, sizeof(hh));
  memset(mi, 0, sizeof(mi));
  memset(ss, 0, sizeof(ss));

  PickNumber(stime, strtime, false, false);

  if (strlen(strtime) != 14) {
    return -1;
  }

  strncpy(yyyy, strtime, 4);
  strncpy(mm, strtime + 4, 2);
  strncpy(dd, strtime + 6, 2);
  strncpy(hh, strtime + 8, 2);
  strncpy(mi, strtime + 10, 2);
  strncpy(ss, strtime + 12, 2);

  struct tm time_str;

  time_str.tm_year = atoi(yyyy) - 1900;
  time_str.tm_mon = atoi(mm) - 1;
  time_str.tm_mday = atoi(dd);
  time_str.tm_hour = atoi(hh);
  time_str.tm_min = atoi(mi);
  time_str.tm_sec = atoi(ss);
  time_str.tm_isdst = 0;

  return mktime(&time_str);
}

CCmdStr::CCmdStr() { m_vCmdStr.clear(); }

CCmdStr::CCmdStr(const std::string &buffer, const char *sepstr,
                 const bool bdelspace) {
  m_vCmdStr.clear();

  SplitToCmd(buffer, sepstr, bdelspace);
}

void CCmdStr::SplitToCmd(const std::string &buffer, const char *sepstr,
                         const bool bdelspace) {
  m_vCmdStr.clear();

  int iPOS = 0;
  std::string srcstr, substr;

  srcstr = buffer;

  while ((iPOS = srcstr.find(sepstr)) >= 0) {
    substr = srcstr.substr(0, iPOS);

    if (bdelspace) {
      char str[substr.length() + 1];
      STRCPY(str, sizeof(str), substr.c_str());

      DeleteLRChar(str, ' ');

      substr = str;
    }

    m_vCmdStr.push_back(substr);

    iPOS = iPOS + strlen(sepstr);

    srcstr = srcstr.substr(iPOS, srcstr.size() - iPOS);
  }

  substr = srcstr;

  if (bdelspace) {
    char str[substr.length() + 1];
    STRCPY(str, sizeof(str), substr.c_str());

    DeleteLRChar(str, ' ');

    substr = str;
  }

  m_vCmdStr.push_back(substr);

  return;
}

int CCmdStr::CmdCount() { return m_vCmdStr.size(); }

bool CCmdStr::GetValue(const int inum, char *value, const int ilen) {
  if ((inum >= (int)m_vCmdStr.size()) || (value == 0)) {
    return false;
  }

  if (ilen > 0) {
    memset(value, 0, ilen + 1);
  }

  if ((m_vCmdStr[inum].length() <= (unsigned int)ilen) || (ilen == 0)) {
    strcpy(value, m_vCmdStr[inum].c_str());
  } else {
    strncpy(value, m_vCmdStr[inum].c_str(), ilen);
    value[ilen] = 0;
  }

  return true;
}

bool CCmdStr::GetValue(const int inum, double *value) {
  if ((inum >= (int)m_vCmdStr.size()) || (value == 0)) {
    return false;
  }

  (*value) = 0;

  if (inum >= (int)m_vCmdStr.size()) {
    return false;
  }

  (*value) = (double)atof(m_vCmdStr[inum].c_str());

  return true;
}

CCmdStr::~CCmdStr() { m_vCmdStr.clear(); }

CFile::CFile() {
  m_fp = NULL;
  m_bEnBuffer = true;
  memset(m_filename, 0, sizeof(m_filename));
  memset(m_filenametmp, 0, sizeof(m_filenametmp));
}

bool CFile::Open(const char *filename, const char *openmode, bool bEnBuffer) {
  Close();

  if ((m_fp = FOPEN(filename, openmode)) == NULL) {
    return false;
  }

  memset(m_filename, 0, sizeof(m_filename));

  STRNCPY(m_filename, sizeof(m_filename), filename, 300);

  m_bEnBuffer = bEnBuffer;

  return true;
}

bool CFile::OpenForRename(const char *filename, const char *openmode,
                          bool bEnBuffer) {
  Close();

  memset(m_filename, 0, sizeof(m_filename));
  STRNCPY(m_filename, sizeof(m_filename), filename, 300);

  memset(m_filenametmp, 0, sizeof(m_filenametmp));
  SNPRINTF(m_filenametmp, sizeof(m_filenametmp), 300, "%s.tmp", m_filename);

  if ((m_fp = FOPEN(m_filenametmp, openmode)) == NULL) {
    return false;
  }

  m_bEnBuffer = bEnBuffer;

  return true;
}

bool CFile::CloseAndRename() {
  if (m_fp == NULL) {
    return false;
  }

  fclose(m_fp);

  m_fp = NULL;

  if (rename(m_filenametmp, m_filename) != 0) {
    remove(m_filenametmp);
    memset(m_filename, 0, sizeof(m_filename));
    memset(m_filenametmp, 0, sizeof(m_filenametmp));
    return false;
  }

  memset(m_filename, 0, sizeof(m_filename));
  memset(m_filenametmp, 0, sizeof(m_filenametmp));

  return true;
}

void CFile::Fprintf(const char *fmt, ...) {
  if (m_fp == NULL) {
    return;
  }

  va_list arg;
  va_start(arg, fmt);
  vfprintf(m_fp, fmt, arg);
  va_end(arg);

  if (!m_bEnBuffer) {
    fflush(m_fp);
  }

  return;
}

bool CFile::Fgets(char *buffer, const int readsize, bool bdelcrt) {
  if (m_fp == NULL) {
    return false;
  }

  memset(buffer, 0, readsize + 1);

  if (fgets(buffer, readsize, m_fp) == NULL) {
    return false;
  }

  if (bdelcrt) {
    DeleteRChar(buffer, '\n');
    DeleteRChar(buffer, '\r');
  }

  return true;
}

void CFile::Close() {
  if (m_fp == NULL) {
    return;
  }

  fclose(m_fp);

  m_fp = NULL;
  memset(m_filename, 0, sizeof(m_filename));

  if (strlen(m_filenametmp) != 0) {
    remove(m_filenametmp);
  }

  memset(m_filenametmp, 0, sizeof(m_filenametmp));

  return;
}

CFile::~CFile() { Close(); }

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

CLogFile::~CLogFile() {
  Close();

  // pthread_spin_destroy(&spin);
}

void CloseIOAndSignal(bool bCloseIO) {
  for (int ii = 0; ii < 64; ++ii) {
    if (bCloseIO) {
      close(ii);
    }

    signal(ii, SIG_IGN);
  }

  return;
}

CSEM::CSEM() {
  m_semid = -1;
  m_sem_flg = SEM_UNDO;
}

bool CSEM::init(key_t key, unsigned short value, short sem_flg) {
  if (m_semid != -1) {
    return false;
  }

  m_sem_flg = sem_flg;

  if ((m_semid = semget(key, 1, 0666)) == -1) {
    if (errno == 2) {
      if ((m_semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL)) == -1) {
        if (errno != EEXIST) {
          perror("init 1 semget()");
          return false;
        }
        if ((m_semid = semget(key, 1, 0666)) == -1) {
          perror("init 2 semget()");
          return false;
        }

        return true;
      }

      union semun sem_union;
      sem_union.val = value;
      if (semctl(m_semid, 0, SETVAL, sem_union) < 0) {
        perror("init semctl()");
        return false;
      }
    } else {
      perror("init 3 semget()");
      return false;
    }
  }

  return true;
}

bool CSEM::P(short sem_op) {
  if (m_semid == -1) {
    return false;
  }

  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = sem_op;
  sem_b.sem_flg = m_sem_flg;
  if (semop(m_semid, &sem_b, 1) == -1) {
    perror("P semop()");
    return false;
  }

  return true;
}

bool CSEM::V(short sem_op) {
  if (m_semid == -1) {
    return false;
  }

  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = sem_op;
  sem_b.sem_flg = m_sem_flg;
  if (semop(m_semid, &sem_b, 1) == -1) {
    perror("V semop()");
    return false;
  }

  return true;
}

int CSEM::value() { return semctl(m_semid, 0, GETVAL); }

bool CSEM::destroy() {
  if (m_semid == -1) {
    return false;
  }

  if (semctl(m_semid, 0, IPC_RMID) == -1) {
    perror("destroy semctl()");
    return false;
  }

  return true;
}

CSEM::~CSEM() {}

CPActive::CPActive() {
  m_shmid = 0;
  m_pos = -1;
  m_shm = 0;
}

bool CPActive::AddPInfo(const int timeout, const char *pname,
                        CLogFile *logfile) {
  if (m_pos != -1) {
    return true;
  }

  if (!m_sem.init(SEMKEYP)) {
    if (logfile != 0) {
      logfile->Write("Create/Get semaphore(%x) falied\n", SEMKEYP);
    } else {
      printf("Create/Get semaphore(%x) falied\n", SEMKEYP);
    }

    return false;
  }

  if ((m_shmid = shmget((key_t)SHMKEYP, MAXNUMP * sizeof(struct st_procinfo),
                        0666 | IPC_CREAT)) == -1) {
    if (logfile != 0) {
      logfile->Write("Create/Get semaphore(%x) falied\n", SHMKEYP);
    } else {
      printf("Create/Get semaphore(%x) falied\n", SHMKEYP);
    }

    return false;
  }

  m_shm = (struct st_procinfo *)shmat(m_shmid, 0, 0);

  struct st_procinfo stprocinfo;
  memset(&stprocinfo, 0, sizeof(stprocinfo));

  stprocinfo.pid = getpid();
  stprocinfo.timeout = timeout;
  stprocinfo.atime = time(0);
  STRNCPY(stprocinfo.pname, sizeof(stprocinfo.pname), pname, 50);

  for (int ii = 0; ii < MAXNUMP; ++ii) {
    if ((m_shm + ii)->pid == stprocinfo.pid) {
      m_pos = ii;
      break;
    }
  }

  m_sem.P();

  if (m_pos == -1) {
    for (int ii = 0; ii < MAXNUMP; ++ii)
      if ((m_shm + ii)->pid == 0) {
        m_pos = ii;
        break;
      }
  }

  if (m_pos == -1) {
    if (logfile != 0) {
      logfile->Write("The shared memory space has been used up\n");
    } else {
      printf("The shared memory space has been used up\n");
    }

    m_sem.V();

    return false;
  }

  memcpy(m_shm + m_pos, &stprocinfo, sizeof(struct st_procinfo));

  m_sem.V();

  return true;
}

bool CPActive::UptATime() {
  if (m_pos == -1) {
    return false;
  }

  (m_shm + m_pos)->atime = time(0);

  return true;
}

CPActive::~CPActive() {
  if (m_pos != -1) {
    memset(m_shm + m_pos, 0, sizeof(struct st_procinfo));
  }

  if (m_shm != 0) {
    shmdt(m_shm);
  }
}
