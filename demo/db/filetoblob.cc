/*
 * filetoblob.cc, 此程序演示开发框架操作 MySQL 数据库
 *                (把图片文件存入 BLOB 字段).
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

  // 准备修改表的 SQL 语句.
  stmt.prepare("update girls set pic=:1 where id=:2");
  stmt.bindinlob(1, stgirls.pic, &stgirls.picsize);
  stmt.bindin(2, &stgirls.id);

  // 修改超女信息表中 id 为 1, 2 的记录.
  for (int ii = 1; ii < 3; ++ii) {
    // 结构体变量初始化.
    memset(&stgirls, 0, sizeof(stgirls));

    // 为结构体变量的成员赋值.
    stgirls.id = ii;  // 超女编号.
    // 把图片的内容加载到 stgirls.pic 中.
    if (ii == 1) {
      stgirls.picsize = filetobuf("1.jpg", stgirls.pic);
    }
    if (ii == 2) {
      stgirls.picsize = filetobuf("2.jpg", stgirls.pic);
    }

    // 执行 SQL 语句, 一定要判断返回值, 0-成功, 其它-失败.
    // 失败代码在 stmt.m_cda.rc 中, 失败描述在 stmt.m_cda.message 中.
    if (stmt.execute() != 0) {
      printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql,
             stmt.m_cda.message);
      return -1;
    }

    // stmt.m_cda.rpc 是本次执行 SQL 影响的记录数.
    printf("成功修改了 %ld 条记录.\n", stmt.m_cda.rpc);
  }

  printf("update table girls ok.\n");

  // 提交数据库事务.
  conn.commit();

  return 0;
}
