# MySQL 头文件存放的目录.
MYSQLINCL = -I/home/erwin/Coding/mysql/include -I/home/erwin/Coding/mini-project/public/db/mysql

# MySQL 库文件存放的目录.
MYSQLLIB = -L/home/erwin/Coding/mysql/lib

# 需要链接的 MySQL 库.
MYSQLLIBS = -lmysqlclient

COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: createtable inserttable updatetable selecttable deletetable filetoblob blobtofile

createtable: createtable.cc
	g++ $(COMPILE_FLAGS) -o createtable createtable.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

inserttable: inserttable.cc
	g++ $(COMPILE_FLAGS) -o inserttable inserttable.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

updatetable: updatetable.cc
	g++ $(COMPILE_FLAGS) -o updatetable updatetable.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

selecttable: selecttable.cc
	g++ $(COMPILE_FLAGS) -o selecttable selecttable.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

deletetable: deletetable.cc
	g++ $(COMPILE_FLAGS) -o deletetable deletetable.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

filetoblob: filetoblob.cc
	g++ $(COMPILE_FLAGS) -o filetoblob filetoblob.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

blobtofile: blobtofile.cc
	g++ $(COMPILE_FLAGS) -o blobtofile blobtofile.cc $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) ../../public/db/mysql/_mysql.cc -lm -lc

clean:
	rm -rf createtable inserttable updatetable selecttable deletetable filetoblob blobtofile
