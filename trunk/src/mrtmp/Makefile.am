INCLUDES = -I@TP_INCLUDE@ -I@TOP_INCLUDE@

# AUTOMAKE_OPTIONS = subdir-objects
lib_LIBRARIES = libmrtmp.a
CXXFLAGS += -Wall
libmrtmp_a_SOURCES = \
	mrtmpconnection.cpp \
	mrtmpurl.cpp \
	mrtmpserver.cpp \
	mrtmphandshake.cpp \
	mrtmpprotocol.cpp \
	mamf0serializer.cpp \
	mamftypes.cpp \
	mrtmpsource.cpp \
	mrtmppool.cpp \
	mflashvideoinfo.cpp \
	mrtmptimestampcorrector.cpp \
	mflvrecoder.cpp \
	BlsConf.cpp \
	mrtmpplayer.cpp \
	BlsChildChannel.cpp \
	BlsMasterChannel.cpp \
	BlsUtils.cpp \
	BlsBackSource.cpp \
	BlsStatistics.cpp \
	BlsHttpService.cpp \
	BlsRtmpPublisher.cpp \
	BlsServerSelector.cpp

noinst_HEADERS = \
	mrtmpconnection.hpp \
	mrtmpurl.hpp \
	mrtmpserver.hpp \
	mrtmphandshake.hpp \
	mrtmpprotocol.hpp \
	mamf0serializer.hpp \
	mamftypes.hpp \
	mrtmpsource.hpp \
	mrtmppool.hpp \
	mflashvideoinfo.hpp \
	mrtmptimestampcorrector.hpp \
	mflvrecoder.hpp \
	BlsConf.hpp \
	mrtmpplayer.hpp \
	BlsChildChannel.hpp \
	BlsMasterChannel.hpp \
	BlsUtils.hpp \
	BlsBackSource.hpp \
	BlsStatistics.hpp \
	BlsHttpService.hpp \
	BlsRtmpPublisher.hpp \
	BlsServerSelector.hpp

all-am:
	@echo -e "\\e[32m"
	@echo "build rtmp lib success."
	@echo -e "\\e[0m"
