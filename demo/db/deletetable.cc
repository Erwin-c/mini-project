/*
 * deletetable.cc, 此程序演示开发框架操作 MySQL 数据库 (删除表中的记录).
 *
 *  Author: Erwin
 */

#include "_mysql.h"  // 开发框架操作 MySQL 的头文件.

int main() {
  connection conn;  // 数据库连接类.

  // 登录数据库, 返回值: 0-成功; 其它是失败, 存放了 MySQL 的错误代码.
  // 失败代码在 conn.m_cda.rc 中, 失败描述在 conn.m_cda.message 中.
  if (conn.connecttodb("127.0.0.1,root,rooterwin,mysql,3306", "utf8") != 0) {
    printf("conn.connecttodb() failed.\n%s\n", conn.m_cda.message);
    return -1;
  }

  sqlstatement stmt(&conn);  // 操作 SQL 语句的对象.

  int iminid, imaxid;  // 删除条件最小和最大的 id.

  // 准备删除表的 SQL 语句.
  stmt.prepare("delete from girls where id>=:1 and id<=:2");

  // 为 SQL 语句绑定输入变量的地址, bindin() 方法不需要判断返回值.
  stmt.bindin(1, &iminid);
  stmt.bindin(2, &imaxid);

  iminid = 1;  // 指定待删除记录的最小 id 的值.
  imaxid = 3;  // 指定待删除记录的最大 id 的值.

  // 执行 SQL 语句, 一定要判断返回值, 0-成功, 其它-失败.
  // 失败代码在 stmt.m_cda.rc 中, 失败描述在 stmt.m_cda.message 中.
  if (stmt.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
    return -1;
  }

  // 请注意, stmt.m_cda.rpc 变量非常重要, 它保存了 SQL 被执行后影响的记录数.
  printf("本次删除了 girls 表 %ld 条记录.\n", stmt.m_cda.rpc);

  // 提交数据库事务.
  conn.commit();

  return 0;
}
