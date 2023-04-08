/*
 * read.c
 *
 *  Author: Erwin
 */

#include "common.h"

size_t readn(int fd, void* buffer, size_t size) {
  char* buffer_pointer = buffer;
  size_t length = size;

  while (length > 0) {
    int result = read(fd, buffer_pointer, length);

    if (result < 0) {
      if (errno == EINTR) {
        continue;  // 考虑非阻塞的情况, 这里需要再次调用 read().
      } else {
        return -1;
      }
    } else if (result == 0) {
      break;  // EOF (End of File) 表示 Socket 关闭.
    }

    length -= result;
    buffer_pointer += result;
  }

  return size - length;  // 返回的是实际读取的字节数.
}
