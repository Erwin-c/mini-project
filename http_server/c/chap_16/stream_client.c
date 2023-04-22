/*
 * stream_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(int argc, char** argv) {
  int socket_fd = 0;
  size_t data_len = 0;
  struct sockaddr_in server_addr = {0};

  if (argc != 2) {
    error(1, 0, "usage: streamclient <IPaddress>");
  }

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "connect failed");
  }

  struct {
    u_int32_t message_length;
    u_int32_t message_type;
    char data[128];
  } message;

  while (fgets(message.data, sizeof(message.data), stdin) != NULL) {
    data_len = strlen(message.data);
    if (message.data[data_len - 1] == '\n') {
      message.data[data_len - 1] = '\0';
    }

    message.message_length = htonl(data_len);
    message.message_type = htonl(1);
    if (write(socket_fd, &message,
              sizeof(message.message_length) + sizeof(message.message_type) +
                  data_len) == -1) {
      error(1, errno, "write failed");
    }
  }

  exit(0);
}
