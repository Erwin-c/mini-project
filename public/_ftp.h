/*
 * _ftp.h
 *
 *  Author: Erwin
 */

#ifndef _FTP_H_
#define _FTP_H_

#include "_public.h"
#include "ftplib.h"

class Cftp {
 public:
  netbuf *m_ftpconn;    // FTP 连接句柄.
  unsigned int m_size;  // 文件的大小, 单位: 字节.
  char m_mtime[21];     // 文件的修改时间, 格式: yyyymmddhh24miss.

  // 以下三个成员变量用于存放 login 方法登录失败的原因.
  bool m_connectfailed;  // 连接失败.
  bool m_loginfailed;  // 登录失败, 用户名和密码不正确, 或没有登录权限.
  bool m_optionfailed;  // 设置传输模式失败.

  Cftp();   // 类的构造函数.
  ~Cftp();  // 类的析构函.

  void initdata();  // 初始化 m_size 和 m_mtime 成员变量.

  // 登录 FTP 服务器.
  // host: FTP 服务器 IP 地址和端口, 中间用 ":" 分隔, 如 "192.168.1.1:21".
  // username: 登录 FTP 服务器用户名.
  // password: 登录 FTP 服务器的密码.
  // imode: 传输模式, 1-FTPLIB_PASSIVE 是被动模式,2-FTPLIB_PORT 是主动模式.
  //                 缺省是被动模式.
  // 返回值: true-成功; false-失败, 获取到的文件时间存放在 m_mtime 成员变量中.
  bool login(const char *host, const char *username, const char *password,
             const int imode = FTPLIB_PASSIVE);

  // 注销.
  bool logout();

  // 获取 FTP 服务器上文件的时间.
  // remotefilename: 待获取的文件名.
  // 返回值: true-成功; false-失败, 获取到的文件时间存放在 m_mtime 成员变量中.
  bool mtime(const char *remotefilename);

  // 获取 FTP 服务器上文件的大小.
  // remotefilename: 待获取的文件名.
  // 返回值: true-成功; false-失败, 获取到的文件大小存放在 m_size 成员变量中.
  bool size(const char *remotefilename);

  // 改变 FTP 服务器的当前工作目录.
  // remotedir: FTP 服务器上的目录名.
  // 返回值: true-成功; false-失败.
  bool chdir(const char *remotedir);

  // 在 FTP 服务器上创建目录.
  // remotedir: FTP 服务器上待创建的目录名.
  // 返回值: true-成功; false-失败.
  bool mkdir(const char *remotedir);

  // 删除 FTP 服务器上的目录.
  // remotedir: FTP 服务器上待删除的目录名.
  // 返回值: true-成功; 如果权限不足, 目录不存在或目录不为空会返回 false.
  bool rmdir(const char *remotedir);

  // 发送 NLST 命令列出 FTP 服务器目录中的子目录名和文件名.
  // remotedir: FTP 服务器的目录名.
  // listfilename: 用于保存从服务器返回的目录和文件名列表.
  // 返回值: true-成功; false-失败.
  // 注意: 如果列出的是 FTP 服务器当前目录, remotedir 用 "", "*", "." 都可以,
  //      但是, 不规范的 FTP 服务器可能有差别.
  bool nlist(const char *remotedir, const char *listfilename);

  // 从 FTP 服务器上获取文件.
  // remotefilename: 待获取 FTP 服务器上的文件名.
  // localfilename: 保存到本地的文件名.
  // bCheckMTime: 文件传输完成后, 是否核对远程文件传输前后的时间,
  //              保证文件的完整性.
  // 返回值: true-成功; false-失败.
  // 注意: 文件在传输的过程中, 采用临时文件命名的方法, 即在 localfilename 后加
  //      ".tmp"， 在传输完成后才正式改为 localfilename.
  bool get(const char *remotefilename, const char *localfilename,
           const bool bCheckMTime = true);

  // 向 FTP 服务器发送文件.
  // localfilename: 本地待发送的文件名.
  // remotefilename: 发送到 FTP 服务器上的文件名.
  // bCheckSize: 文件传输完成后, 是否核对本地文件和远程文件的大小,
  //             保证文件的完整性.
  // 返回值: true-成功; false-失败.
  // 注意: 文件在传输的过程中, 采用临时文件命名的方法, 即在 remotefilename 后加
  //      ".tmp"， 在传输完成后才正式改为 remotefilename.
  bool put(const char *localfilename, const char *remotefilename,
           const bool bCheckSize = true);

  // 删除 FTP 服务器上的文件.
  // remotefilename: 待删除的 FTP 服务器上的文件名.
  // 返回值: true-成功; false-失败.
  bool ftpdelete(const char *remotefilename);

  // 重命名 FTP 服务器上的文件.
  // srcremotefilename: FTP 服务器上的原文件名.
  // dstremotefilename: FTP 服务器上的目标文件名.
  // 返回值: true-成功; false-失败.
  bool ftprename(const char *srcremotefilename, const char *dstremotefilename);

  /* 以下三个方法如果理解不了就算了, 可以不启用. */
  // 发送 LIST 命令列出 FTP 服务器目录中的文件.
  // 参数和返回值与 nlist() 相同.
  bool dir(const char *remotedir, const char *listfilename);

  // 向 FTP 服务器发送 site 命令.
  // command: 命令的内容.
  // 返回值: true-成功; false-失败.
  bool site(const char *command);

  // 获取服务器返回信息的最后一条
  // (return a pointer to the last response received).
  char *response();
};

#endif  // _FTP_H_
