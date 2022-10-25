COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: demo01 demo02 demo03 demo04 demo05 demo06 demo07 demo08 demo10 demo11 demo12\
     demo13 demo14 demo31 demo32 demo33 demo20 demo26 demo27 demo28 tcpselect client\
     tcppoll tcpepoll

demo01: demo01.cc
	g++ $(COMPILE_FLAGS) -o demo01 demo01.cc -lm -lc

demo02: demo02.cc
	g++ $(COMPILE_FLAGS) -o demo02 demo02.cc -lm -lc

demo03: demo03.cc
	g++ $(COMPILE_FLAGS) -o demo03 demo03.cc -lm -lc

demo04: demo04.cc
	g++ $(COMPILE_FLAGS) -o demo04 demo04.cc -lm -lc

demo05: demo05.cc
	g++ $(COMPILE_FLAGS) -o demo05 demo05.cc ./public/_public.cc -lm -lc

demo06: demo06.cc
	g++ $(COMPILE_FLAGS) -o demo06 demo06.cc ./public/_public.cc -lm -lc

demo07: demo07.cc
	g++ $(COMPILE_FLAGS) -o demo07 demo07.cc ./public/_public.cc -lm -lc

demo08: demo08.cc
	g++ $(COMPILE_FLAGS) -o demo08 demo08.cc ./public/_public.cc -lm -lc

demo10: demo10.cc
	g++ $(COMPILE_FLAGS) -o demo10 demo10.cc ./public/_public.cc -lm -lc

demo11: demo11.cc
	g++ $(COMPILE_FLAGS) -o demo11 demo11.cc ./public/_public.cc -lm -lc

demo12: demo12.cc
	g++ $(COMPILE_FLAGS) -o demo12 demo12.cc ./public/_public.cc -lm -lc

demo13: demo13.cc
	g++ $(COMPILE_FLAGS) -o demo13 demo13.cc ./public/_public.cc -lm -lc

demo14: demo14.cc
	g++ $(COMPILE_FLAGS) -o demo14 demo14.cc ./public/_public.cc -lm -lc

demo31: demo31.cc
	g++ $(COMPILE_FLAGS) -o demo31 demo31.cc ./public/_public.cc -lm -lc

demo32: demo32.cc
	g++ $(COMPILE_FLAGS) -o demo32 demo32.cc ./public/_public.cc -lm -lc

demo33: demo33.cc
	g++ $(COMPILE_FLAGS) -o demo33 demo33.cc ./public/_public.cc -lm -lc

demo20: demo20.cc
	g++ $(COMPILE_FLAGS) -o demo20 demo20.cc ./public/_public.cc -lpthread -lm -lc

demo26: demo26.cc
	g++ $(COMPILE_FLAGS) -o demo26 demo26.cc ./public/_public.cc -lm -lc

demo27: demo27.cc
	g++ $(COMPILE_FLAGS) -o demo27 demo27.cc ./public/_public.cc -lm -lc

demo28: demo28.cc
	g++ $(COMPILE_FLAGS) -o demo28 demo28.cc -I/project/public /project/public/_public.cc -I/oracle/home/rdbms/public -I/project/public/db/oracle -L/oracle/home/lib -L. -lclntsh /project/public/db/oracle/_ooci.cc -lm -lc

client: client.cc
	g++ $(COMPILE_FLAGS) -o client client.cc ./public/_public.cc -lm -lc

tcpselect: tcpselect.cc
	g++ $(COMPILE_FLAGS) -o tcpselect tcpselect.cc ./public/_public.cc -lm -lc

tcppoll: tcppoll.cc
	g++ $(COMPILE_FLAGS) -o tcppoll tcppoll.cc ./public/_public.cc -lm -lc

tcpepoll: tcpepoll.cc
	g++ $(COMPILE_FLAGS) -o tcpepoll tcpepoll.cc ./public/_public.cc -lm -lc

clean:
	rm -f demo01 demo02 demo03 demo04 demo05 demo06 demo07 demo08 demo10 demo11 demo12
	rm -f demo13 demo14 demo31 demo32 demo33 demo20 demo26 demo27 demo28 tcpselect client
	rm -f tcppoll tcpepoll
