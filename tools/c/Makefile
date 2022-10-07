#### PROJECT SETTINGS ####

PUBINCL = -I../../public

PUBCPP = ../../public/_public.cc

# General compiler flags
COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: procctl checkproc gzipfiles deletefiles ftpgetfiles

procctl: procctl.cc
	g++ $(COMPILE_FLAGS) -o procctl procctl.cc
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

ftpgetfiles: ftpgetfiles.cc
	g++ $(COMPILE_FLAGS) -o ftpgetfiles ftpgetfiles.cc $(PUBINCL) $(PUBCPP) ../../public/libftp.a ../../public/_ftp.cc -lm -lc
	cp ftpgetfiles ../bin/.

clean:
	rm procctl checkproc gzipfiles deletefiles ftpgetfiles
	rm ../bin/procctl ../bin/checkproc ../bin/gzipfiles ../bin/deletefiles ../bin/ftpgetfiles
