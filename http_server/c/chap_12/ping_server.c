/*
 * ping_server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"
#include "message_object.h"

int count;

void sig_int(int signo) {
  printf("signo: %d\n", signo);
  printf("\nreceived %d datagrams\n", count);
  exit(0);
}

int main(int argc, char** argv) {
  int listen_fd = 0, conn_fd = 0;
  int sleepingTime = 0;
  socklen_t client_len = 0;
  messageObject message = {0}, pong_message = {0};
  struct sockaddr_in server_addr = {0}, client_addr = {0};

  if (argc != 2) {
    error(1, 0, "usage: tcpsever <sleepingtime>");
  }

  sleepingTime = atoi(argv[1]);

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERV_PORT);

  if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    error(1, errno, "bind failed");
  }

  if (listen(listen_fd, LISTENQ) < 0) {
    error(1, errno, "listen failed ");
  }

  signal(SIGINT, sig_int);
  signal(SIGPIPE, SIG_IGN);

  if ((conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr,
                        &client_len)) < 0) {
    error(1, errno, "bind failed ");
  }

  for (;;) {
    ssize_t read_rc = read(conn_fd, &message, sizeof(messageObject));
    if (read_rc < 0) {
      error(1, errno, "read failed");
    } else if (read_rc == 0) {
      error(1, 0, "client closed\n");
    }

    printf("received %ld bytes\n", read_rc);
    ++count;

    switch (ntohl(message.type)) {
      case MSG_TYPE1:
        printf("process MSG_TYPE1\n");
        break;

      case MSG_TYPE2:
        printf("process MSG_TYPE2\n");
        break;

      case MSG_PING:
        pong_message.type = MSG_PONG;
        sleep(sleepingTime);

        ssize_t write_rc = write(conn_fd, &pong_message, sizeof(pong_message));
        if (write_rc < 0) {
          error(1, errno, "write failed");
        }
        break;

      default:
        error(1, 0, "unknown message type (%d)\n", ntohl(message.type));
    }
  }

  exit(0);
}
