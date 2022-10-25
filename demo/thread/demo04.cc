/*
 * demo04.cc, 本程序演示线程参数的传递(用结构体的地址传递多个参数).
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct st_args {
  int no;         // 线程编号.
  char name[51];  // 线程名.
};

// 线程的主函数.
void *thmain(void *arg);

int main() {
  pthread_t thid = 0;

  // 创建线程.
  st_args *stargs = new st_args;
  stargs->no = 15;
  strcpy(stargs->name, "Test thread");
  if (pthread_create(&thid, NULL, thmain, (void *)stargs) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  printf("Join...\n");
  pthread_join(thid, NULL);
  printf("Join ok.\n");

  return 0;
}

void *thmain(void *arg) {
  st_args *pst = (st_args *)arg;
  printf("no = %d, name = %s\n", pst->no, pst->name);
  delete pst;
  printf("Thread begins to run.\n");

  pthread_exit(0);
}
