/*
 * acceptor.cpp
 *
 *  Author: Erwin
 */

#include "acceptor.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdlib>
#include <cstring>

Acceptor::Acceptor(int port) {
  _listen_port = port;
  _listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  fcntl(_listen_fd, F_SETFL, O_NONBLOCK);

  sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  int on = 1;
  setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  int rt = bind(_listen_fd, reinterpret_cast<sockaddr*>(&server_addr),
                sizeof(server_addr));
  if (rt < 0) {
    exit(1);
  }

  rt = listen(_listen_fd, LISTENQ);
  if (rt < 0) {
    exit(1);
  }

  // signal(SIGPIPE, SIG_IGN);
}
