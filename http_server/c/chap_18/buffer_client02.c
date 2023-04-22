/*
 * buffer_client02.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(int argc, char** argv) {
  int socket_fd = 0;
  u_int32_t len = 0;
  char buf[128] = "just for fun";

  if (argc != 2) {
    error(1, 0, "usage: bufferclient02 <IPaddress>");
  }

  socket_fd = tcp_client(argv[1], SERV_PORT);

  struct {
    u_int32_t message_length;
    u_int32_t message_type;
    char data[128];
  } message;

  len = 65535;  // error.
  message.message_length = htonl(len);
  message.message_type = htonl(1);

  strncpy(message.data, buf, strlen(buf));
  if (write(socket_fd, (char*)&message,
            sizeof(message.message_length) + sizeof(message.message_type) +
                strlen(message.data)) == -1) {
    error(1, errno, "send failed");
  }

  sleep(100);

  exit(0);
}
