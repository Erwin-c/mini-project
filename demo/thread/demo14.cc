/*
 * demo14.cc, 本程序演示线程同步-读写锁.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;  // 声明读写锁并初始化.

// 信号 15 的处理函数.
void handle(int sig);

// 线程主函数.
void *thmain(void *arg);

int main() {
  // 设置信号 15 的处理函数.
  signal(15, handle);

  pthread_t thid1, thid2, thid3;

  // 创建线程.
  if (pthread_create(&thid1, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  sleep(1);
  if (pthread_create(&thid2, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  sleep(1);
  if (pthread_create(&thid3, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  pthread_join(thid3, NULL);

  // 销毁锁.
  pthread_rwlock_destroy(&rwlock);

  return 0;
}

void handle(int sig) {
  printf("开始申请写锁...\n");

  // 加锁.
  pthread_rwlock_wrlock(&rwlock);
  printf("申请写锁成功.\n\n");

  sleep(10);

  // 解锁.
  pthread_rwlock_unlock(&rwlock);
  printf("写锁已释放.\n\n");

  return;
}

void *thmain(void *arg) {
  for (int ii = 0; ii < 100; ++ii) {
    printf("线程 %lu 开始申请读锁...\n", pthread_self());

    // 加锁.
    pthread_rwlock_rdlock(&rwlock);
    printf("线程 %lu 开始申请读锁成功.\n\n", pthread_self());

    sleep(5);

    // 解锁.
    pthread_rwlock_unlock(&rwlock);
    printf("线程 %lu 已释放读锁.\n\n", pthread_self());

    if (ii == 3) {
      sleep(8);
    }
  }

  return (void *)1;
}
