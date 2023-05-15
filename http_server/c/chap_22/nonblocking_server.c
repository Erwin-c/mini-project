/*
 * nonblocking_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

#define FD_INIT_SIZE 128

// 数据缓冲区.
struct Buffer {
  int connect_fd;        // 连接字 Socket.
  char buffer[MAXLINE];  // 实际缓冲.
  size_t writeIndex;     // 缓冲写入位置.
  size_t readIndex;      // 缓冲读取位置.
  int readable;          // 是否可以读.
};

char rot13_char(char c) {
  if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) {
    return c + 13;
  } else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) {
    return c - 13;
  } else {
    return c;
  }
}

// 分配一个 Buffer 对象, 初始化 writeIdnex 和 readIndex 等.
struct Buffer* alloc_Buffer(void) {
  struct Buffer* buffer = NULL;
  buffer = malloc(sizeof(struct Buffer));
  if (buffer == NULL) {
    return NULL;
  }

  buffer->connect_fd = 0;
  buffer->writeIndex = buffer->readIndex = buffer->readable = 0;

  return buffer;
}

// 释放 Buffer 对象.
void free_Buffer(struct Buffer* buffer) {
  free(buffer);
  return;
}

// 这里从 fd Socket 读取数据, 数据先读取到本地 buf 数组中, 再逐个拷贝到 buffer
// 对象缓冲中.
int onSocketRead(int fd, struct Buffer* buffer) {
  ssize_t result = 0;
  char buf[MAXLINE] = {0};

  while (1) {
    result = read(fd, buf, sizeof(buf) - 1);
    if (result <= 0) {
      break;
    }

    buf[result] = '\0';

    // 按 char 对每个字节进行拷贝, 每个字节都会先调用 rot13_char 来完成编码,
    // 之后拷贝到 buffer 对象的缓冲中, 其中 writeIndex 标志了缓冲中写的位置.
    for (int i = 0; i < result; ++i) {
      if (buffer->writeIndex < sizeof(buffer->buffer)) {
        buffer->buffer[buffer->writeIndex++] = rot13_char(buf[i]);
      }
      // 如果读取了回车符, 则认为 client 端发送结束,
      // 此时可以把编码后的数据回送给客户端.
      if (buf[i] == '\n') {
        buffer->readable = 1;  // 缓冲区可以读
      }
    }
  }

  if (result == 0) {
    return 1;
  } else if (result == -1) {
    if (errno == EAGAIN) {
      return 0;
    }

    return -1;
  }

  return 0;
}

// 从 buffer 对象的 readIndex 开始读, 一直读到 writeIndex 的位置,
// 这段区间是有效数据.
int onSocketWrite(int fd, struct Buffer* buffer) {
  while (buffer->readIndex < buffer->writeIndex) {
    ssize_t result = 0;
    result = write(fd, buffer->buffer + buffer->readIndex,
                   buffer->writeIndex - buffer->readIndex);
    if (result == -1) {
      if (errno == EAGAIN) {
        return 0;
      }

      return -1;
    }

    buffer->readIndex += result;
  }

  // readIndex 已经追上 writeIndex, 说明有效发送区间已经全部读完，将 readIndex
  // 和 writeIndex 设置为 0, 复用这段缓冲.
  if (buffer->readIndex == buffer->writeIndex) {
    buffer->readIndex = buffer->writeIndex = 0;
  }

  // 缓冲数据已经全部读完, 不需要再读.
  buffer->readable = 0;

  return 0;
}

int main(void) {
  struct Buffer* buffer[FD_INIT_SIZE] = {0};
  int i = 0;
  for (i = 0; i < FD_INIT_SIZE; ++i) {
    buffer[i] = alloc_Buffer();
  }

  int listen_fd = 0;
  listen_fd = tcp_nonblocking_server_listen(SERV_PORT);

  fd_set readset = {0}, writeset = {0};
  FD_ZERO(&readset);
  FD_ZERO(&writeset);

  while (1) {
    int maxfd = listen_fd;

    FD_ZERO(&readset);
    FD_ZERO(&writeset);

    // listener 加入 readset.
    FD_SET(listen_fd, &readset);

    for (i = 0; i < FD_INIT_SIZE; ++i) {
      if (buffer[i]->connect_fd > 0) {
        if (buffer[i]->connect_fd > maxfd) {
          maxfd = buffer[i]->connect_fd;
        }

        FD_SET(buffer[i]->connect_fd, &readset);

        if (buffer[i]->readable != 0) {
          FD_SET(buffer[i]->connect_fd, &writeset);
        }
      }
    }

    if (select(maxfd + 1, &readset, &writeset, NULL, NULL) == -1) {
      error(1, errno, "select failed");
    }

    if (FD_ISSET(listen_fd, &readset)) {
      printf("listening socket readable\n");
      sleep(5);

      int conn_fd = 0;
      struct sockaddr_storage ss = {0};
      socklen_t slen = 0;
      conn_fd = accept(listen_fd, (struct sockaddr*)&ss, &slen);
      if (conn_fd == -1) {
        error(1, errno, "accept failed");
      } else if (conn_fd > FD_INIT_SIZE) {
        error(1, 0, "too many connections");
        close(conn_fd);
      } else {
        make_nonblocking(conn_fd);

        if (buffer[conn_fd]->connect_fd == 0) {
          buffer[conn_fd]->connect_fd = conn_fd;
        } else {
          error(1, 0, "too many connections");
        }
      }
    }

    for (i = 0; i < maxfd + 1; ++i) {
      int rc = 0;
      if (i == listen_fd) {
        continue;
      }

      if (FD_ISSET(i, &readset)) {
        rc = onSocketRead(i, buffer[i]);
      }

      if (rc == 0 && FD_ISSET(i, &writeset)) {
        rc = onSocketWrite(i, buffer[i]);
      }

      if (rc == -1) {
        buffer[i]->connect_fd = 0;
        close(i);
      }
    }
  }

  exit(0);
}
