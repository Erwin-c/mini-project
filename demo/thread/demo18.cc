/*
 * demo18.cc, 本程序演示用互斥锁和条件变量实现高速缓存.
 *
 *  Author: Erwin
 */

#include <pthread.h>
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

vector<st_message> vcache;  // 用 vector 容器做缓存.

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  // 声明并初始化条件变量.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 声明并初始化互斥锁.

// 生产者, 数据入队.
void incache(int sig);

// 在这里释放关闭文件, 断开网络连接, 回滚数据库事务, 释放锁等等.
void thcleanup(void *arg);

// 消费者, 数据出队线程的主函数.
void *outcache(void *arg);

int main() {
  // 接收 15 的信号, 调用生产者函数.
  signal(15, incache);

  // 创建三个消费者线程.
  pthread_t thid1, thid2, thid3;
  pthread_create(&thid1, NULL, outcache, NULL);
  pthread_create(&thid2, NULL, outcache, NULL);
  pthread_create(&thid3, NULL, outcache, NULL);

  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  pthread_join(thid3, NULL);

  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);

  return 0;
}

void incache(int sig) {
  static int mesgid = 1;  // 消息的计数器.

  st_message stmesg;  // 消息内容.
  memset(&stmesg, 0, sizeof(st_message));

  // 给缓存队列加锁.
  pthread_mutex_lock(&mutex);

  // 生产数据, 放入缓存队列.
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);
  stmesg.mesgid = ++mesgid;
  vcache.push_back(stmesg);

  // 给缓存队列解锁.
  pthread_mutex_unlock(&mutex);

  // 发送条件信号, 激活一个线程.
  // pthread_cond_signal(&cond);

  // 发送条件信号, 激活全部的线程.
  pthread_cond_broadcast(&cond);

  return;
}

void thcleanup(void *arg) {
  printf("cleanup ok.\n");

  pthread_mutex_unlock(&mutex);

  // A condition wait (whether timed or not) is a cancellation point.
  // When the cancelability type of a thread is set to PTHREAD_CAN_CEL_DEFERRED,
  // a side-effect of acting upon a cancellation request while in a condition
  // wait is that the mutex is (in effect) re-acquired before calling the
  // first cancellation cleanup handler. The effect is as if the thread were
  // unblocked, allowed to execute up to the point of returning from the call to
  // pthread_cond_timedwait() or pthread_cond_wait(), but at that point notices
  // the cancellation request and instead of returning to the caller of
  // pthread_cond_timedwait() or pthread_cond_wait(), starts the thread
  // cancellation activities, which includes calling cancellation cleanup
  // handlers.
  // 意思就是在 pthread_cond_wait() 时执行 pthread_cancel() 后,
  // 要先在线程清理函数中要先解锁已与相应条件变量绑定的 mutex,
  // 这样是为了保证 pthread_cond_wait() 可以返回到调用线程.
};

void *outcache(void *arg) {
  // 把线程清理函数入栈.
  pthread_cleanup_push(thcleanup, NULL);

  st_message stmesg;  // 用于存放出队的消息.

  while (true) {
    // 给缓存队列加锁.
    pthread_mutex_lock(&mutex);

    // 如果缓存队列为空, 等待, 用 while 防止条件变量虚假唤醒.
    while (vcache.size() == 0) {
      pthread_cond_wait(&cond, &mutex);
    }

    // 从缓存队列中获取第一条记录, 然后删除该记录.
    // 内存拷贝.
    memcpy(&stmesg, &vcache[0], sizeof(st_message));
    vcache.erase(vcache.begin());

    // 给缓存队列解锁.
    pthread_mutex_unlock(&mutex);

    // 以下是处理业务的代码.
    printf("phid = %ld, mesgid = %d\n", pthread_self(), stmesg.mesgid);
    usleep(100);
  }

  pthread_cleanup_pop(1);  // 把线程清理函数出栈.

  return (void *)1;
}
