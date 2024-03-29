/*
 * common.h
 *
 *  Author: Erwin
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <arpa/inet.h>  // inet(3) functions.
#include <errno.h>
#include <fcntl.h>  // for nonblocking.
#include <netdb.h>
#include <netinet/in.h>  // sockaddr_in{} and other Internet defns.
#include <poll.h>        // for convenience.
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // for convenience.
#include <sys/ioctl.h>
#include <sys/select.h>  // for convenience.
#include <sys/socket.h>  // basic socket definitions.
#include <sys/stat.h>    // for S_xxx file mode constants.
// #include <sys/sysctl.h>
#include <sys/time.h>   // timeval{} for select().
#include <sys/types.h>  // basic system data types.
#include <sys/uio.h>    // for iovec{} and readv/writev.
#include <sys/un.h>     // for Unix domain sockets.
#include <sys/wait.h>
#include <time.h>  // timespec{} for pselect().
#include <unistd.h>

// #include "channel_map.h"
// #include "config.h"
// #include "inetaddress.h"
// #include "log.h"
// #include "tcp_server.h"

#ifdef EPOLL_ENABLE
#include <sys/epoll.h>
#endif

void err_dump(const char *, ...);

void err_msg(const char *, ...);

void err_quit(const char *, ...);

void err_ret(const char *, ...);

void err_sys(const char *, ...);

void error(int status, int err, char *fmt, ...);

char *sock_ntop(const struct sockaddr_in *sin, socklen_t salen);

ssize_t readn(int fd, void *vptr, size_t n);

ssize_t read_message(int fd, void *buffer, size_t length);

size_t readline(int fd, char *buffer, size_t length);

int tcp_server(int port);

int tcp_server_listen(int port);

int tcp_nonblocking_server_listen(int port);

void make_nonblocking(int fd);

int tcp_client(char *address, int port);

#define SERV_PORT 43211
#define MAXLINE 4096
#define UNIXSTR_PATH "/var/lib/unixstream.sock"
#define LISTENQ 1024
#define BUFFER_SIZE 4096

#endif  // COMMON_H_
