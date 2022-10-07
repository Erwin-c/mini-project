all: lib_public.a lib_public.so libftp.a libftp.so

lib_public.a:_public.h _public.cc
	g++ -c -o lib_public.a _public.cc

lib_public.so:_public.h _public.cc
	g++ -fPIC -shared -o lib_public.so _public.cc

libftp.a:ftplib.h ftplib.c
	gcc -c -o libftp.a ftplib.c

libftp.so:ftplib.h ftplib.c
	gcc -fPIC -shared -o libftp.so ftplib.c

clean:
	rm -f lib_public.a lib_public.so libftp.a libftp.so
