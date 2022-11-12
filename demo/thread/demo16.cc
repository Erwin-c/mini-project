/*
 * demo16.cc, 本程序演示线程同步-信号量.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var;

sem_t sem;  // 声明信号量.

// 线程主函数.
void *thmain(void *arg);

int main() {
  // 初始化信号量.
  sem_init(&sem, 0, 1);

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
  printf("join...\n");
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  printf("join ok.\n");

  printf("var = %d\n", var);

  // 销毁信号量.
  sem_destroy(&sem);

  return 0;
}

void *thmain(void *arg) {
  for (int ii = 0; ii < 1000000; ++ii) {
    // 加锁.
    sem_wait(&sem);

    ++var;

    // 解锁.
    sem_post(&sem);
  }

  return (void *)1;
}
