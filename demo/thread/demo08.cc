/*
 * demo08.cc, 本程序演示线程资源的回收(线程清理函数).
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 线程清理函数 1.
void thcleanup1(void *arg);
// 线程清理函数 2.
void thcleanup2(void *arg);
// 线程清理函数 3.
void thcleanup3(void *arg);

// 线程主函数.
void *thmain(void *arg);

int main() {
  pthread_t thid;

  // 创建线程.
  if (pthread_create(&thid, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  int result;
  void *ret;

  printf("Join...\n");
  result = pthread_join(thid, &ret);
  printf("thid result = %d, ret = %d\n", result, (int)(long)ret);
  printf("join ok.\n");

  return 0;
}

void thcleanup1(void *arg) {
  // 在这里释放关闭文件, 断开网络连接, 回滚数据库事务, 释放锁等等.
  printf("cleanup1() ok.\n");

  return;
};

void thcleanup2(void *arg) {
  // 在这里释放关闭文件, 断开网络连接, 回滚数据库事务, 释放锁等等.
  printf("cleanup2() ok.\n");

  return;
};

void thcleanup3(void *arg) {
  // 在这里释放关闭文件, 断开网络连接, 回滚数据库事务, 释放锁等等.
  printf("cleanup3() ok.\n");

  return;
};

void *thmain(void *arg) {
  // 把线程清理函数 1 入栈(关闭文件指针).
  pthread_cleanup_push(thcleanup1, NULL);
  // 把线程清理函数 2 入栈(关闭 Socket).
  pthread_cleanup_push(thcleanup2, NULL);
  // 把线程清理函数 3 入栈(回滚数据库事务).
  pthread_cleanup_push(thcleanup3, NULL);

  for (int ii = 0; ii < 3; ++ii) {
    sleep(1);
    printf("pthmain() sleep(%d) ok.\n", ii + 1);
  }

  // 把线程清理函数 3 出栈.
  pthread_cleanup_pop(3);
  // 把线程清理函数 2 出栈.
  pthread_cleanup_pop(2);
  // 把线程清理函数 1 出栈.
  pthread_cleanup_pop(1);

  return (void *)1;
}
