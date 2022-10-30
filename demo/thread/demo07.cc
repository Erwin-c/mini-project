/*
 * demo07.cc, 本程序演示线程资源的回收(分离线程).
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 线程主函数.
void *thmain(void *arg);

int main() {
  pthread_t thid;

  pthread_attr_t attr;  // 申明线程属性的数据结构.
  // 初始化.
  pthread_attr_init(&attr);
  // 设置线程的属性.
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  // 创建线程.
  if (pthread_create(&thid, &attr, thmain, NULL) != 0) {
    printf("pthread_create failed.\n");
    exit(-1);
  }

  // 销毁数据结构.
  pthread_attr_destroy(&attr);

  // pthread_detach(thid);

  sleep(5);

  int result;
  void *ret;

  printf("Join...\n");
  result = pthread_join(thid, &ret);
  printf("thid result = %d, ret = %d\n", result, (int)(long)ret);
  printf("Join ok.\n");

  return 0;
}

void *thmain(void *arg) {
  // pthread_detach(pthread_self());

  for (int ii = 0; ii < 3; ++ii) {
    sleep(1);
    printf("pthmain sleep(%d) ok.\n", ii + 1);
  }

  return (void *)1;
}
