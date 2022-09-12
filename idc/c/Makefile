#### PROJECT SETTINGS ####

# General compiler flags
COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: crtmetdata

crtmetdata: crtmetdata.cc
	g++ $(COMPILE_FLAGS) -o crtmetdata crtmetdata.cc ../../public/_public.cc -I../../public
	cp crtmetdata ../bin/.

clean:
	rm crtmetdata
	rm ../bin/crtmetdata
