/*
 * acceptor.hpp
 *
 *  Author: Erwin
 */

#ifndef __ACCEPTOR_HPP_
#define __ACCEPTOR_HPP_

#define LISTENQ 1024

class Acceptor {
 public:
  Acceptor(int port);

 private:
  int _listen_port;
  int _listen_fd;
};

#endif  // __ACCEPTOR_HPP_
