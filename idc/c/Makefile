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

all: crtsurfdata obtcodetodb obtmindtodb

crtsurfdata: crtsurfdata.cc
	g++ $(COMPILE_FLAGS) -o crtsurfdata crtsurfdata.cc ../../public/_public.cc -I../../public
	cp crtsurfdata ../bin/.

obtcodetodb: obtcodetodb.cc
	g++ $(COMPILE_FLAGS) -o obtcodetodb obtcodetodb.cc $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc
	cp obtcodetodb ../bin/.

obtmindtodb: obtmindtodb.cc idcapp.h idcapp.cc
	g++ $(COMPILE_FLAGS) -o obtmindtodb obtmindtodb.cc $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) idcapp.cc -lm -lc
	cp obtmindtodb ../bin/.

clean:
	rm -f crtsurfdata obtcodetodb obtmindtodb
