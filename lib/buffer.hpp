/*
 * buffer.hpp
 *
 *  Author: Erwin
 */

#ifndef __BUFFER_HPP_
#define __BUFFER_HPP_

#define INIT_BUFFER_SIZE 65536

#include <cstddef>
#include <cstdint>

// 数据缓冲区.
class Buffer {
 public:
  static const char* CRLF;

  Buffer();

  ~Buffer();

  size_t buffer_readable_size();

  // 往 Buffer 里写数据.
  void buffer_append(void* data, size_t size);

  // 往 Buffer 里写数据.
  void buffer_append(char data);

  // 往 Buffer 里写数据.
  void buffer_append(char* data);

  // 读 Socket 数据, 往 Buffer 里写.
  int buffer_socket_read(int fd);

  // 读 Buffer 数据.
  char buffer_read_char();

  // 查询 Buffer 数据.
  char* buffer_find_CRLF();

 private:
  char* _data;          // 实际缓冲.
  size_t _total_size;   // 总大小.
  size_t _read_index;   // 缓冲读取位置.
  size_t _write_index;  // 缓冲写入位置.

  size_t buffer_writeable_size();

  size_t buffer_front_spare_size();

  void make_room(size_t size);
};

#endif  // __BUFFER_HPP_
