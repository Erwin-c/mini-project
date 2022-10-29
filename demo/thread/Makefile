COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: demo01 demo02 demo03 demo04 demo05

demo01: demo01.cc
	g++ $(COMPILE_FLAGS) -o demo01 demo01.cc -lpthread

demo02: demo02.cc
	g++ $(COMPILE_FLAGS) -o demo02 demo02.cc -lpthread

demo03: demo03.cc
	g++ $(COMPILE_FLAGS) -o demo03 demo03.cc -lpthread

demo04: demo04.cc
	g++ $(COMPILE_FLAGS) -o demo04 demo04.cc -lpthread 

demo05: demo05.cc
	g++ $(COMPILE_FLAGS) -o demo05 demo05.cc -lpthread 

clean:
	rm -f demo01 demo02 demo03 demo04 demo05
