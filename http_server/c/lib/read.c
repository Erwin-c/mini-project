/*
 * read.c
 *
 *  Author: Erwin
 */

#include "common.h"

ssize_t readn(int fd, void* buffer, size_t size) {
  char* buffer_pointer = buffer;
  size_t length = size;
  ssize_t read_rc = 0;

  while (length > 0) {
    read_rc = read(fd, buffer_pointer, length);

    if (read_rc < 0) {
      if (errno == EINTR) {
        continue;  // 考虑非阻塞的情况, 这里需要再次调用 read().
      } else {
        return -1;
      }
    } else if (read_rc == 0) {
      break;  // EOF (End of File) 表示 Socket 关闭.
    }

    length -= read_rc;
    buffer_pointer += read_rc;
  }

  return size - length;  // 返回的是实际读取的字节数.
}

ssize_t read_message(int fd, void* buffer, size_t length) {
  u_int32_t msg_length = 0;
  u_int32_t msg_type = 0;
  ssize_t read_rc = 0;

  // Retrieve the length of the record.
  read_rc = readn(fd, &msg_length, sizeof(msg_length));
  if (read_rc != sizeof(msg_length)) {
    return read_rc < 0 ? -1 : 0;
  }

  msg_length = ntohl(msg_length);

  read_rc = readn(fd, &msg_type, sizeof(msg_type));
  if (read_rc != sizeof(msg_type)) {
    return read_rc < 0 ? -1 : 0;
  }

  // 判断 buffer 是否可以容纳下数据.
  if (msg_length > length) {
    return -1;
  }

  // Retrieve the record itself.
  read_rc = readn(fd, buffer, msg_length);
  if (read_rc != msg_length) {
    return read_rc < 0 ? -1 : 0;
  }

  return read_rc;
}
