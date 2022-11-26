# 开发框架头文件路径.
PUBINCL = -I/home/erwin/Coding/mini-project/public

# 开发框架 cc 文件名, 这里直接包含进来, 没有采用链接库, 是为了方便调试.
PUBCPP = /home/erwin/Coding/mini-project/public/_public.cc

# MySQL 头文件存放的目录.
MYSQLINCL = -I/home/erwin/Coding/mysql/include -I/home/erwin/Coding/mini-project/public/db/mysql

# MySQL 库文件存放的目录.
MYSQLLIB = -L/home/erwin/Coding/mysql/lib

# 需要链接的 MySQL 库.
MYSQLLIBS = -lmysqlclient

# 开发框架 MySQL 的 cc 文件名, 这里直接包含进来, 没有采用链接库, 是为了方便调试.
MYSQLCPP = /home/erwin/Coding/mini-project/public/db/mysql/_mysql.cc

# 编译参数.
COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: procctl checkproc gzipfiles deletefiles tcpgetfiles tcpputfiles fileserver execsql

procctl: procctl.cc
	g++ $(COMPILE_FLAGS) -o procctl procctl.cc $(PUBINCL) $(PUBCPP)
	cp procctl ../bin/.

checkproc: checkproc.cc
	g++ $(COMPILE_FLAGS) -o checkproc checkproc.cc $(PUBINCL) $(PUBCPP) -lm -lc
	cp checkproc ../bin/.

gzipfiles: gzipfiles.cc
	g++ $(COMPILE_FLAGS) -o gzipfiles gzipfiles.cc $(PUBINCL) $(PUBCPP) -lm -lc
	cp gzipfiles ../bin/.

deletefiles: deletefiles.cc
	g++ $(COMPILE_FLAGS) -o deletefiles deletefiles.cc $(PUBINCL) $(PUBCPP) -lm -lc
	cp deletefiles ../bin/.

tcpgetfiles: tcpgetfiles.cc
	g++ $(COMPILE_FLAGS) -o tcpgetfiles tcpgetfiles.cc $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpgetfiles ../bin/.

tcpputfiles: tcpputfiles.cc
	g++ $(COMPILE_FLAGS) -o tcpputfiles tcpputfiles.cc $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpputfiles ../bin/.

fileserver: fileserver.cc
	g++ $(COMPILE_FLAGS) -o fileserver fileserver.cc $(PUBINCL) $(PUBCPP) -lm -lc
	cp fileserver ../bin/.

execsql: execsql.cc
	g++ $(CFLAGS) -o execsql execsql.cc $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp execsql ../bin/.

clean:
	rm procctl checkproc gzipfiles deletefiles tcpgetfiles tcpputfiles fileserver execsql
