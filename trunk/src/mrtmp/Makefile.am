INCLUDES = -I@TP_INCLUDE@ -I@TOP_INCLUDE@

# AUTOMAKE_OPTIONS = subdir-objects
lib_LIBRARIES = libmrtmp.a
CXXFLAGS += -Wall
libmrtmp_a_SOURCES = \
	mrtmpconnection.cpp \
	BlsRtmpUrl.cpp \
	BlsRtmpServer.cpp \
	mrtmphandshake.cpp \
	BlsRtmpProtocol.cpp \
	mamf0serializer.cpp \
	mamftypes.cpp \
	BlsRtmpSource.cpp \
	BlsConsumer.cpp \
	mflashvideoinfo.cpp \
	BlsTimestampCorrector.cpp \
	mflvrecoder.cpp \
	BlsConf.cpp \
	BlsRtmpPlayer.cpp \
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
	BlsRtmpUrl.hpp \
	BlsRtmpServer.hpp \
	mrtmphandshake.hpp \
	BlsRtmpProtocol.hpp \
	mamf0serializer.hpp \
	mamftypes.hpp \
	BlsRtmpSource.hpp \
	BlsConsumer.hpp \
	mflashvideoinfo.hpp \
	BlsTimestampCorrector.hpp \
	mflvrecoder.hpp \
	BlsConf.hpp \
	BlsRtmpPlayer.hpp \
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
