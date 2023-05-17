/*
 * thread01.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

extern void loop_echo(int);

void* thread_run(void* arg) {
  pthread_detach(pthread_self());

  int fd = *(int*)arg;
  loop_echo(fd);

  return NULL;
}

int main(void) {
  int listener_fd = tcp_server_listen(SERV_PORT);

  while (1) {
    struct sockaddr_storage ss;
    socklen_t slen;
    int fd = accept(listener_fd, (struct sockaddr*)&ss, &slen);
    if (fd == -1) {
      error(1, errno, "accept failed");
    }

    pthread_t tid;
    pthread_create(&tid, NULL, thread_run, (void*)&fd);
  }

  exit(0);
}
