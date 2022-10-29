/*
 * demo05.cc, 本程序演示线程线程退出(终止)的状态.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct st_ret {
  int retcode;         // 返回代码.
  char message[1024];  // 返回内容.
};

// 线程的主函数.
void *thmain(void *arg);

int main() {
  pthread_t thid = 0;

  // 创建线程.
  if (pthread_create(&thid, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  printf("Join...\n");
  st_ret *pst = NULL;
  pthread_join(thid, (void **)&pst);
  printf("retcode = %d, message = %s\n", pst->retcode, pst->message);
  delete pst;
  printf("Join ok.\n");

  return 0;
}

void *thmain(void *arg) {
  printf("Thread begins to run.\n");

  // 注意, 如果用结构体的地址作为线程的返回值,
  // 必须保存在线程主函数结束后地址仍是有效的.
  // 所以, 要采用动态分配内存的方法, 不能用局部变量.
  st_ret *ret = new st_ret;
  ret->retcode = 1121;
  strcpy(ret->message, "Test");

  pthread_exit((void *)ret);
}
