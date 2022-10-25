/*
 * demo12.cc, 本程序演示线程同步-互斥锁.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 声明互斥锁.

// 线程主函数.
void *thmain(void *arg);

int main() {
  // 初始化互斥锁.
  // pthread_mutex_init(&mutex, NULL);

  pthread_t thid1, thid2;

  // 创建线程.
  if (pthread_create(&thid1, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  if (pthread_create(&thid2, NULL, thmain, NULL) != 0) {
    printf("(pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  printf("Join...\n");
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  printf("Join ok.\n");

  printf("var = %d\n", var);

  // 销毁锁.
  pthread_mutex_destroy(&mutex);

  return 0;
}

void *thmain(void *arg) {
  for (int ii = 0; ii < 1000000; ++ii) {
    // 加锁.
    pthread_mutex_lock(&mutex);
    ++var;
    // 解锁.
    pthread_mutex_unlock(&mutex);
  }

  return (void *)1;
}
