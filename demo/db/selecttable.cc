/*
 * selecttable.cc, 此程序演示开发框架操作 MySQL 数据库 (查询表中的记录).
 *
 *  Author: Erwin
 */

#include <unistd.h>

#include "_mysql.h"  // 开发框架操作MySQL的头文件.

int main() {
  connection conn;  // 数据库连接类。

  // 登录数据库, 返回值: 0-成功; 其它是失败, 存放了 MySQL 的错误代码.
  // 失败代码在 conn.m_cda.rc 中, 失败描述在 conn.m_cda.message 中.
  if (conn.connecttodb("127.0.0.1,root,rooterwin,mysql,3306", "utf8") != 0) {
    printf("conn.connecttodb() failed.\n%s\n", conn.m_cda.message);
    return -1;
  }

  // 定义用于超女信息的结构, 与表中的字段对应.
  struct st_girls {
    long id;         // 超女编号.
    char name[31];   // 超女姓名.
    double weight;   // 超女体重.
    char btime[20];  // 报名时间.
  } stgirls;

  sqlstatement stmt(&conn);  // 操作 SQL 语句的对象.

  int iminid, imaxid;  // 查询条件最小和最大的 id.

  // 准备查询表的 SQL 语句.
  stmt.prepare(
      "\
    select id,name,weight,date_format(btime,'%%Y-%%m-%%d %%H:%%i:%%s') from girls where id>=:1 and id<=:2");

  /*
   * 注意事项:
   * 1. 如果 SQL 语句的主体没有改变, 只需要 prepare() 一次就可以了;
   * 2. 结果集中的字段, 调用 bindout() 绑定变量的地址;
   * 3. bindout() 方法的返回值固定为 0, 不用判断返回值;
   * 4. 如果 SQL 语句的主体已改变, prepare() 后, 需重新用 bindout() 绑定变量;
   * 5. 调用 execute() 方法执行 SQL 语句, 然后再循环调用 next() 方法获取结果集
   *    中的记录;
   * 6. 每调用一次 next() 方法, 从结果集中获取一条记录, 字段内容保存在已绑定的
   *    变量中.
   */

  // 为 SQL 语句绑定输入变量的地址, bindin() 方法不需要判断返回值.
  stmt.bindin(1, &iminid);
  stmt.bindin(2, &imaxid);
  // 为 SQL 语句绑定输出变量的地址, bindout() 方法不需要判断返回值.
  stmt.bindout(1, &stgirls.id);
  stmt.bindout(2, stgirls.name, 30);
  stmt.bindout(3, &stgirls.weight);
  stmt.bindout(4, stgirls.btime, 19);

  iminid = 1;  // 指定待查询记录的最小 id 的值.
  imaxid = 3;  // 指定待查询记录的最大 id 的值.

  // 执行 SQL 语句, 一定要判断返回值, 0-成功, 其它-失败.
  // 失败代码在 stmt.m_cda.rc 中, 失败描述在 stmt.m_cda.message 中.
  if (stmt.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
    return -1;
  }

  // 本程序执行的是查询语句, 执行stmt.execute() 后,
  // 将会在数据库的缓冲区中产生一个结果集.
  while (true) {
    memset(&stgirls, 0, sizeof(stgirls));  // 结构体变量初始化.

    // 从结果集中获取一条记录, 一定要判断返回值, 0-成功, 1403-无记录, 其它-失败.
    // 在实际开发中, 除了 0 和 1403, 其它的情况极少出现.
    if (stmt.next() != 0) {
      break;
    }

    // 把获取到的记录的值打印出来.
    printf("id = %ld, name = %s, weight = %.02f, btime = %s\n", stgirls.id,
           stgirls.name, stgirls.weight, stgirls.btime);
  }

  // 请注意, stmt.m_cda.rpc 变量非常重要, 它保存了 SQL 被执行后影响的记录数.
  printf("本次查询了 girls 表 %ld 条记录.\n", stmt.m_cda.rpc);

  return 0;
}
