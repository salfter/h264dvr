bin_PROGRAMS=hello

hello_SOURCES=hello.cpp
hello_LDADD=$(WX_LIBS)

if !WIN32_BUILD

bin_PROGRAMS+= recd

recd_SOURCES=recd.cpp
recd_CXXFLAGS=-I../include
recd_LDADD=../lib/librecd.a ../lib/libdaemonize.a -lrt

endif
