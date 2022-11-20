# MySQL 头文件存放的目录.
MYSQLINCL = -I/home/erwin/Coding/mysql/include

# MySQL 库文件存放的目录.
MYSQLLIB = -L/home/erwin/Coding/mysql/lib

# 需要链接的 MySQL 库.
MYSQLLIBS = -lmysqlclient

COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: createtable

createtable: createtable.cc ../../public/db/mysql/_mysql.h ../../public/db/mysql/_mysql.cc
	g++ $(COMPILE_FLAGS) -o createtable createtable.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

clean:
	rm -rf createtable
