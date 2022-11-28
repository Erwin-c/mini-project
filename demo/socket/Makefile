# 开发框架头文件路径.
PUBINCL = -I/home/erwin/Coding/mini-project/public

# 开发框架 cc 文件名, 这里直接包含进来, 没有采用链接库, 是为了方便调试.
PUBCPP = /home/erwin/Coding/mini-project/public/_public.cc

# 编译参数.
COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: demo01 demo02 demo03 demo04 demo05 demo06 demo07 demo08 demo10 demo11 demo12\
     demo13 demo14 demo20 client tcpselect tcppoll tcpepoll

demo01: demo01.cc
	g++ $(COMPILE_FLAGS) -o demo01 demo01.cc -lm -lc

demo02: demo02.cc
	g++ $(COMPILE_FLAGS) -o demo02 demo02.cc -lm -lc

demo03: demo03.cc
	g++ $(COMPILE_FLAGS) -o demo03 demo03.cc -lm -lc

demo04: demo04.cc
	g++ $(COMPILE_FLAGS) -o demo04 demo04.cc -lm -lc

demo05: demo05.cc
	g++ $(COMPILE_FLAGS) -o demo05 demo05.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo06: demo06.cc
	g++ $(COMPILE_FLAGS) -o demo06 demo06.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo07: demo07.cc
	g++ $(COMPILE_FLAGS) -o demo07 demo07.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo08: demo08.cc
	g++ $(COMPILE_FLAGS) -o demo08 demo08.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo10: demo10.cc
	g++ $(COMPILE_FLAGS) -o demo10 demo10.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo11: demo11.cc
	g++ $(COMPILE_FLAGS) -o demo11 demo11.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo12: demo12.cc
	g++ $(COMPILE_FLAGS) -o demo12 demo12.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo13: demo13.cc
	g++ $(COMPILE_FLAGS) -o demo13 demo13.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo14: demo14.cc
	g++ $(COMPILE_FLAGS) -o demo14 demo14.cc $(PUBINCL) $(PUBCPP) -lm -lc

demo20: demo20.cc
	g++ $(COMPILE_FLAGS) -o demo20 demo20.cc $(PUBINCL) $(PUBCPP) -lm -lc

client: client.cc
	g++ $(COMPILE_FLAGS) -o client client.cc $(PUBINCL) $(PUBCPP) -lm -lc

tcpselect: tcpselect.cc
	g++ $(COMPILE_FLAGS) -o tcpselect tcpselect.cc $(PUBINCL) $(PUBCPP) -lm -lc

tcppoll: tcppoll.cc
	g++ $(COMPILE_FLAGS) -o tcppoll tcppoll.cc $(PUBINCL) $(PUBCPP) -lm -lc

tcpepoll: tcpepoll.cc
	g++ $(COMPILE_FLAGS) -o tcpepoll tcpepoll.cc $(PUBINCL) $(PUBCPP) -lm -lc

clean:
	rm -f demo01 demo02 demo03 demo04 demo05 demo06 demo07 demo08 demo10 demo11 demo12 demo13 demo14 demo20 client tcpselect tcppoll tcpepoll
