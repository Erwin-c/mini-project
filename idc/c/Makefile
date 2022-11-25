#### PROJECT SETTINGS ####

# General compiler flags
COMPILE_FLAGS = -std=c++2a -g -Wall -Wextra

all: crtsurfdata

crtsurfdata: crtsurfdata.cc
	g++ $(COMPILE_FLAGS) -o crtsurfdata crtsurfdata.cc ../../public/_public.cc -I../../public
	cp crtsurfdata ../bin/.

clean:
	rm crtsurfdata
	rm ../bin/crtsurfdata
