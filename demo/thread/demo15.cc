/*
 * demo15.cc, 本程序演示线程同步-条件变量.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  // 声明条件变量并初始化.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 声明互斥锁并初始化.

// 线程主函数.
void *thmain(void *arg);

// 信号 15 的处理函数.
void handle(int sig);

int main() {
  // 设置信号 15 的处理函数.
  signal(15, handle);

  pthread_t thid1, thid2, thid3;

  // 创建线程.
  if (pthread_create(&thid1, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  if (pthread_create(&thid2, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  if (pthread_create(&thid3, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  pthread_join(thid3, NULL);

  // 销毁条件变量.
  pthread_cond_destroy(&cond);
  // 销毁互斥锁.
  pthread_mutex_destroy(&mutex);

  return 0;
}

void *thmain(void *arg) {
  while (true) {
    printf("线程 %lu 开始等待条件信号...\n", pthread_self());

    // 等待条件信号.
    pthread_cond_wait(&cond, &mutex);

    printf("线程 %lu 等待条件信号成功.\n\n", pthread_self());
  }

  return (void *)1;
}

void handle(int sig) {
  printf("发送条件信号...\n");

  // 唤醒等待条件变量的一个线程.
  // pthread_cond_signal(&cond);

  // 唤醒等待条件变量的全部线程.
  pthread_cond_broadcast(&cond);

  return;
}
