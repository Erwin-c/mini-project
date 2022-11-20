/*
 * createtable.cc, 此程序演示开发框架操作 MySQL 数据库 (创建表).
 *
 *  Author: Erwin
 */

#include "../../public/db/mysql/_mysql.h"  // 开发框架操作 MySQL 的头文件.

int main(int argc, char *argv[]) {
  connection conn;  // 数据库连接类.

  // 登录数据库, 返回值: 0-成功; 其它是失败, 存放了 MySQL 的错误代码.
  // 失败代码在 conn.m_cda.rc 中, 失败描述在 conn.m_cda.message 中.
  if (conn.connecttodb("127.0.0.1,root,mysqlpwd,mysql,3306", "utf8") != 0) {
    printf("conn.connecttodb() 失败.\n%s\n", conn.m_cda.message);
    return -1;
  }

  sqlstatement stmt(&conn);  // 操作 SQL 语句的对象.

  // 准备创建表的 SQL 语句.
  // 超女表 girls, 超女编号 id, 超女姓名 name, 体重 weight,
  // 报名时间 btime, 超女说明 memo, 超女图片 pic.
  stmt.prepare(
      "create table girls(id      bigint(10),\
                   name    varchar(30),\
                   weight  decimal(8,2),\
                   btime   datetime,\
                   memo    longtext,\
                   pic     longblob,\
                   primary key (id))");
  /*
   * 1. int prepare(const char *fmt, ...), SQL 语句可以多行书写.
   * 2. SQL 语句最后的分号可有可无, 建议不要写 (兼容性考虑).
   * 3. SQL 语句中不能有说明文字.
   * 4. 可以不用判断 stmt.prepare() 的返回值, stmt.execute() 时再判断.
   */

  // 执行 SQL语句, 一定要判断返回值: 0-成功; 其它-失败.
  // 失败代码在 stmt.m_cda.rc 中, 失败描述在 stmt.m_cda.message 中.
  if (stmt.execute() != 0) {
    printf("stmt.execute() 失败.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
    return -1;
  }

  printf("create table girls 成功.\n");

  return 0;
}

/*
-- 超女基本信息表.
create table girls(id      bigint(10),    -- 超女编号.
                   name    varchar(30),   -- 超女姓名.
                   weight  decimal(8,2),  -- 超女体重.
                   btime   datetime,      -- 报名时间.
                   memo    longtext,      -- 备注.
                   pic     longblob,      -- 照片.
                   primary key (id));
*/
