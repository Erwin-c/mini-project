/*
 * demo19.cc, 本程序演示只用信号量实现高速缓存.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <vector>

using namespace std;

// 缓存队列消息的结构体.
struct st_message {
  int mesgid;          // 消息的 id.
  char message[1024];  // 消息的内容.
};

vector<struct st_message> vcache;  // 用 vector 容器做缓存.

sem_t notify, lock;  // 声明信号量.

// 生产者, 数据入队.
void incache(int sig);
// 消费者, 数据出队线程的主函数.
void *outcache(void *arg);

int main() {
  // 接收 15 的信号, 调用生产者函数.
  signal(15, incache);

  // 初始化通知的信号量, 第 3 个参数为 0.
  sem_init(&notify, 0, 0);
  // 初始化加锁的信号量, 第 3 个参数为 1.
  sem_init(&lock, 0, 1);

  // 创建三个消费者线程.
  pthread_t thid1, thid2, thid3;
  pthread_create(&thid1, NULL, outcache, NULL);
  pthread_create(&thid2, NULL, outcache, NULL);
  pthread_create(&thid3, NULL, outcache, NULL);

  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  pthread_join(thid3, NULL);

  sem_destroy(&notify);
  sem_destroy(&lock);

  return 0;
}

void incache(int sig) {
  static int mesgid = 1;  // 消息的计数器.

  st_message stmesg;  // 消息内容.
  memset(&stmesg, 0, sizeof(st_message));

  sem_wait(&lock);

  // 生产数据，放入缓存队列。
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);

  sem_post(&lock);

  // 把信号量的值加 1, 将唤醒消费者线程.
  sem_post(&notify);
  sem_post(&notify);
  sem_post(&notify);
  sem_post(&notify);

  return;
}

void *outcache(void *arg) {
  st_message stmesg;  // 用于存放出队的消息.

  while (true) {
    // 给缓存队列加锁.
    sem_wait(&lock);

    // 如果缓存队列为空, 等待, 用 while 防止虚假唤醒.
    while (vcache.size() == 0) {
      // 给缓存队列解锁.
      sem_post(&lock);
      // 等待信号量的值大于 0.
      sem_wait(&notify);
      // 给缓存队列加锁.
      sem_wait(&lock);
    }

    // 从缓存队列中获取第一条记录, 然后删除该记录.
    // 内存拷贝.
    memcpy(&stmesg, &vcache[0], sizeof(st_message));
    vcache.erase(vcache.begin());

    // 给缓存队列解锁.
    sem_post(&lock);

    // 以下是处理业务的代码.
    printf("phid = %ld, mesgid = %d\n", pthread_self(), stmesg.mesgid);
    usleep(100);
  }

  return (void *)1;
}
