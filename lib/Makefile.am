noinst_LIBRARIES=

if !WIN32_BUILD

noinst_LIBRARIES+=librecd.a libdaemonize.a

librecd_a_SOURCES=DiskWriter.cpp recd_capture.cpp
librecd_a_CXXFLAGS=-I../include

libdaemonize_a_SOURCES=daemonize.cpp
libdaemonize_a_CXXFLAGS=-I../include

endif
