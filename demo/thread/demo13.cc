/*
 * demo11.cc, 本程序演示线程同步-自旋锁.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var;

pthread_spinlock_t spin;  // 声明自旋锁.

// 线程主函数.
void *thmain(void *arg);

int main() {
  // 初始化自旋锁.
  pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);

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
  pthread_spin_destroy(&spin);

  return 0;
}

void *thmain(void *arg) {
  for (int ii = 0; ii < 1000000; ++ii) {
    // 加锁.
    pthread_spin_lock(&spin);
    ++var;
    // 解锁.
    pthread_spin_unlock(&spin);
  }

  return (void *)1;
}
