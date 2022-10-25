/*
 * demo01.cc, 本程序演示线程的创建和终止.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var = 0;

void fun1();

// 线程主函数.
void *thmain1(void *arg);

void fun2();

// 线程主函数.
void *thmain2(void *arg);

int main() {
  pthread_t thid1 = 0;  // 线程 id, typedef unsigned long pthread_t.
  pthread_t thid2 = 0;

  // 创建线程.
  if (pthread_create(&thid1, NULL, thmain1, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  if (pthread_create(&thid2, NULL, thmain2, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  printf("Join...\n");
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  printf("Join ok.\n");

  return 0;
}

void fun1() { return; }

void *thmain1(void *arg) {
  for (int ii = 0; ii < 5; ++ii) {
    var = ii + 1;
    sleep(1);
    printf("pthmain1() sleep(%d) ok.\n", var);
    if (ii == 2) {
      fun1();
    }
  }

  pthread_exit(0);
}

void fun2() { pthread_exit(0); }

void *thmain2(void *arg) {
  for (int ii = 0; ii < 5; ++ii) {
    sleep(1);
    printf("pthmain2() sleep(%d) ok.\n", var);
    if (ii == 2) {
      fun2();
    }
  }
}
