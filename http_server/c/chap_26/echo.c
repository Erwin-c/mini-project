/*
 * echo.c
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

void loop_echo(int fd) {
  char outbuf[MAXLINE] = {0};
  size_t outbuf_used = 0;

  while (1) {
    char ch;
    ssize_t result = recv(fd, &ch, 1, 0);

    // 断开连接或者出错.
    if (result == 0) {
      break;
    } else if (result == -1) {
      error(1, errno, "recv error");
    }

    if (outbuf_used < MAXLINE) {
      outbuf[outbuf_used++] = rot13_char(ch);
    }

    if (ch == '\n') {
      send(fd, outbuf, outbuf_used, 0);
      outbuf_used = 0;
      continue;
    }
  }

  return;
}
