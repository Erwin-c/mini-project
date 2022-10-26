/*
 * demo17.cc, 本程序演示线程同步-条件变量,
 *            pthread_cond_wait(&cond, &mutex)函数中发生了什么?
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

// 线程 1 主函数.
void *thmain1(void *arg);
// 线程 2 主函数.
void *thmain2(void *arg);

int main() {
  pthread_t thid1, thid2;

  // 创建线程.
  if (pthread_create(&thid1, NULL, thmain1, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  sleep(1);

  if (pthread_create(&thid2, NULL, thmain2, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);

  // 销毁条件变量.
  pthread_cond_destroy(&cond);
  // 销毁互斥锁.
  pthread_mutex_destroy(&mutex);

  return 0;
}

void *thmain1(void *arg) {
  printf("线程一申请互斥锁...\n");
  pthread_mutex_lock(&mutex);
  printf("线程一申请互斥锁成功.\n");

  printf("线程一开始等待条件信号...\n");
  // 等待条件信号.
  pthread_cond_wait(&cond, &mutex);
  printf("线程一等待条件信号成功.\n");

  return (void *)1;
}

void *thmain2(void *arg) {
  printf("线程二申请互斥锁...\n");
  pthread_mutex_lock(&mutex);
  printf("线程二申请互斥锁成功.\n");

  pthread_cond_signal(&cond);

  sleep(5);

  printf("线程二解锁.\n");
  pthread_mutex_unlock(&mutex);

  return 0;

  printf("线程二申请互斥锁...\n");
  pthread_mutex_lock(&mutex);
  printf("线程二申请互斥锁成功.\n");

  printf("线程二开始等待条件信号...\n");
  // 等待条件信号.
  pthread_cond_wait(&cond, &mutex);
  printf("线程二等待条件信号成功.\n");

  return (void *)1;
}
