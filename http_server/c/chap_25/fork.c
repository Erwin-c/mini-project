/*
 * epoll_server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char rot13_char(char c) {
  if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) {
    return c + 13;
  } else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) {
    return c - 13;
  } else {
    return c;
  }
}

void child_run(int fd) {
  char outbuf[MAXLINE] = {0};
  size_t outbuf_used = 0;

  while (1) {
    char ch = '\0';
    ssize_t rc = 0;
    rc = recv(fd, &ch, 1, 0);
    if (rc == 0) {
      break;
    } else if (rc == -1) {
      error(1, errno, "recv failed");
    }

    /* We do this test to keep the user from overflowing the buffer. */
    if (outbuf_used < MAXLINE) {
      outbuf[outbuf_used++] = rot13_char(ch);
    }

    if (ch == '\n') {
      if (send(fd, outbuf, outbuf_used, 0) == -1) {
        error(1, errno, "send failed");
      }

      outbuf_used = 0;
      continue;
    }
  }

  return;
}

void sigchld_handler(int sig) {
  printf("sig: %d\n", sig);

  while (waitpid(-1, NULL, WNOHANG) > 0) {
    ;
  }

  return;
}

int main(void) {
  int listen_fd = 0;
  listen_fd = tcp_server_listen(SERV_PORT);

  signal(SIGCHLD, sigchld_handler);

  while (1) {
    int fd = 0;
    struct sockaddr_storage ss = {0};
    socklen_t slen = 0;
    fd = accept(listen_fd, (struct sockaddr*)&ss, &slen);
    if (fd == -1) {
      error(1, errno, "accept failed");
    }

    if (fork() == 0) {
      close(listen_fd);
      child_run(fd);
      exit(0);
    } else {
      close(fd);
    }
  }

  exit(0);
}
