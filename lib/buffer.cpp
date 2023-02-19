/*
 * buffer.cpp
 *
 *  Author: Erwin
 */

#include "buffer.hpp"

#include <sys/uio.h>

#include <cstdlib>
#include <cstring>

const char* Buffer::CRLF = "\r\n";

Buffer::Buffer() {
  _data = static_cast<char*>(malloc(INIT_BUFFER_SIZE));
  _total_size = INIT_BUFFER_SIZE;
  _read_index = 0;
  _write_index = 0;
}

Buffer::~Buffer() { free(_data); }

size_t Buffer::buffer_readable_size() { return _write_index - _read_index; }

void Buffer::buffer_append(void* data, size_t size) {
  if (data == nullptr) {
    return;
  }

  make_room(size);
  // 拷贝数据到可写空间中.
  memcpy(_data + _write_index, data, size);
  _write_index += size;

  return;
}

void Buffer::buffer_append(char data) {
  make_room(1);
  // 拷贝数据到可写空间中.
  _data[_write_index++] = data;

  return;
}

void Buffer::buffer_append(char* data) {
  if (data == nullptr) {
    return;
  }

  size_t size = strlen(data);
  buffer_append(data, size);

  return;
}

int Buffer::buffer_socket_read(int fd) {
  char additional_buffer[INIT_BUFFER_SIZE];
  size_t max_writable = buffer_writeable_size();
  iovec vec[2];
  vec[0].iov_base = _data + _write_index;
  vec[0].iov_len = max_writable;
  vec[1].iov_base = additional_buffer;
  vec[1].iov_len = sizeof(additional_buffer);

  ssize_t result = readv(fd, vec, 2);
  if (result < 0) {
    return -1;
  }

  if (static_cast<size_t>(result) <= max_writable) {
    _write_index += result;
  } else {
    _write_index = _total_size;
    buffer_append(additional_buffer, result - max_writable);
  }

  return result;
}

char Buffer::buffer_read_char() { return _data[_read_index++]; }

char* Buffer::buffer_find_CRLF() {
  return static_cast<char*>(
      memmem(_data + _read_index, buffer_readable_size(), CRLF, 2));
}

size_t Buffer::buffer_writeable_size() { return _total_size - _write_index; }

size_t Buffer::buffer_front_spare_size() { return _read_index; }

void Buffer::make_room(size_t size) {
  if (buffer_writeable_size() >= size) {
    return;
  }

  // 如果 front_spare 和 writeable 的大小加起来可以容纳数据,
  // 则把可读数据往前面拷贝.
  if (buffer_front_spare_size() + buffer_writeable_size() >= size) {
    size_t readable = buffer_readable_size();
    for (size_t i = 0; i < readable; ++i) {
      _data[i] = _data[_read_index + i];
    }
    _read_index = 0;
    _write_index = readable;
  } else {
    // 扩大缓冲区.
    char* tmp = static_cast<char*>(realloc(_data, _total_size + size));
    if (tmp == nullptr) {
      return;
    }
    _data = tmp;
    _total_size += size;
  }

  return;
}
