INCLUDES = -I@TP_INCLUDE@ -I@TOP_INCLUDE@

AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libmcore.a
CXXFLAGS += -Wall

libmcore_a_SOURCES = merrno.cpp \
	mglobal.cpp \
	mobject.cpp \
	mthread.cpp \
	mcoreapplication.cpp \
	mdir.cpp \
    mfile.cpp \
    mstring.cpp \
    mstringlist.cpp \
    mtimer.cpp \
	crypto/md5/md5.cpp \
	mmd5.cpp \
    mdate.cpp \
    mcore.cpp \
    melapsedtimer.cpp \
	mstream.cpp \
	mprocess.cpp \
	mloger.cpp \
	mconf.cpp \
	mvariant.cpp \
	mhash.cpp

mcoredir = ${includedir}/mcore
mcore_HEADERS = merrno.hpp \
	mglobal.hpp \
	mobject.hpp \
	mthread.hpp \
	mcoreapplication.hpp \
	mfile.hpp \
	mdir.hpp \
	mstring.hpp \
	mstringlist.hpp \
	mtimer.hpp \
	mmd5.hpp \
	mdate.hpp \
	mcore.hpp \
	melapsedtimer.hpp \
	mstream.hpp \
	mprocess.hpp \
	mloger.hpp \
	mconf.hpp \
	mvariant.hpp \
	mhash.hpp

all-am:
	@echo -e "\\e[32m"
	@echo "build mcore success."
	@echo -e "\\e[0m"
