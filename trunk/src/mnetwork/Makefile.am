INCLUDES = -I@TP_INCLUDE@ -I@TOP_INCLUDE@

AUTOMAKE_OPTIONS = foreign
CXXFLAGS += -Wall
lib_LIBRARIES = libmnetwork.a
libmnetwork_a_SOURCES = mhostinfo.cpp \
    mtcpsocket.cpp \
    mhttpparser.cpp \
    murl.cpp \
    mtcpserver.cpp \
    mhttpreactor.cpp \
    mhttpheader.cpp \
    mhttpresponseheader.cpp \
    mhttprequestheader.cpp

mnetworkdir = ${includedir}/CmNetwork
mnetwork_HEADERS = mhostinfo.hpp \
    mtcpsocket.hpp \
    mhttpparser.hpp \
    murl.hpp \
    mtcpserver.hpp \
    mhttpreactor.hpp \
    mhttpheader.hpp \
    mhttpresponseheader.hpp \
    mhttprequestheader.hpp

all-am:
	@echo -e "\\e[32m"
	@echo "build mnetwork success."
	@echo -e "\\e[0m"
