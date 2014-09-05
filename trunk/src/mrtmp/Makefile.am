INCLUDES = -I@TP_INCLUDE@ -I@TOP_INCLUDE@

# AUTOMAKE_OPTIONS = subdir-objects
lib_LIBRARIES = libmrtmp.a
CXXFLAGS += -Wall
libmrtmp_a_SOURCES = \
	mrtmpconnection.cpp \
	mrtmpplayer.cpp \
	mrtmpsocket.cpp \
	mrtmpurl.cpp \
	mrtmpserver.cpp \
	srs_core_handshake.cpp \
	mrtmpprotocol.cpp \
	mamf0serializer.cpp \
	mamfvariant.cpp \
	mamftypes.cpp \
	mrtmpsource.cpp \
	mrtmppool.cpp \
	mflashvideoinfo.cpp \
	mrtmptimestampcorrector.cpp

noinst_HEADERS = \
	mrtmpconnection.hpp \
	mrtmpplayer.hpp \
	mrtmpsocket.hpp \
	mrtmpurl.hpp \
	mrtmpserver.hpp \
	srs_core_handshake.hpp \
	mrtmpprotocol.hpp \
	mamf0serializer.hpp \
	mamfvariant.hpp \
	mamftypes.hpp \
	mrtmpsource.hpp \
	mrtmppool.hpp \
	mflashvideoinfo.hpp \
	mrtmptimestampcorrector.hpp

all-am:
	@echo -e "\\e[32m"
	@echo "build rtmp lib success."
	@echo -e "\\e[0m"
