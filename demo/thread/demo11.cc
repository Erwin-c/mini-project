/*
 * demo11.cc, 本程序演示线程安全.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <atomic>
#include <iostream>

std::atomic<int> var;

// 线程主函数.
void *thmain(void *arg);

int main() {
  pthread_t thid1, thid2;

  // 创建线程.
  if (pthread_create(&thid1, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  if (pthread_create(&thid2, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  printf("Join...\n");
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  printf("Join ok.\n");

  // printf("var = %d\n", var);
  std::cout << "var = " << var << std::endl;

  return 0;
}

void *thmain(void *arg) {
  for (int ii = 0; ii < 1000000; ++ii) {
    ++var;
    // __sync_fetch_and_add(&var, 1);
  }

  return (void *)1;
}
