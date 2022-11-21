/*
 * blobtofile.cc, 此程序演示开发框架操作MySQL数据库
 *                (提取 BLOB 字段内容到图片文件中).
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

  // 定义用于超女信息的结构, 与表中的字段对应.
  struct st_girls {
    long id;                // 超女编号.
    char pic[100000];       // 超女图片的内容.
    unsigned long picsize;  // 图片内容占用的字节数.
  } stgirls;

  sqlstatement stmt(&conn);  // 操作 SQL 语句的对象.

  // 准备查询表的SQL语句。
  stmt.prepare("select id,pic from girls where id in (1,2)");
  stmt.bindout(1, &stgirls.id);
  stmt.bindoutlob(2, stgirls.pic, 100000, &stgirls.picsize);

  // 执行 SQL 语句, 一定要判断返回值, 0-成功, 其它-失败.
  // 失败代码在 stmt.m_cda.rc 中, 失败描述在 stmt.m_cda.message 中.
  if (stmt.execute() != 0) {
    printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
    return -1;
  }

  // 本程序执行的是查询语句, 执行 stmt.execute() 后,
  // 将会在数据库的缓冲区中产生一个结果集.
  while (true) {
    // 先把结构体变量初始化.
    memset(&stgirls, 0, sizeof(stgirls));

    // 从结果集中获取一条记录, 一定要判断返回值, 0-成功, 1403-无记录, 其它-失败.
    // 在实际开发中, 除了 0 和 1403, 其它的情况极少出现.
    if (stmt.next() != 0) {
      break;
    }

    // 生成文件名.
    char filename[101];
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%ld_out.jpg", stgirls.id);

    // 把内容写入文件.
    buftofile(filename, stgirls.pic, stgirls.picsize);
  }

  // 请注意, stmt.m_cda.rpc 变量非常重要, 它保存了 SQL 被执行后影响的记录数.
  printf("本次查询了 girls 表 %ld 条记录.\n", stmt.m_cda.rpc);

  return 0;
}
