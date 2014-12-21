INCLUDES = -I@TP_INCLUDE@ -I@TOP_INCLUDE@

bin_PROGRAMS = bls

bls_LDFLAGS = -ldl
bls_LDFLAGS += -lz
bls_LDADD  = ../mnetwork/libmnetwork.a
bls_LDADD += ../mcore/libmcore.a
bls_LDADD += @TP_LIBS@
bls_SOURCES = main.cpp

CXXFLAGS += -Wall
bls_SOURCES = \
	main.cpp \
	BlsRtmpConnection.cpp \
	BlsRtmpUrl.cpp \
	BlsRtmpServer.cpp \
	BlsRtmpHandshake.cpp \
	BlsRtmpProtocol.cpp \
	BlsAMF0Serializer.cpp \
	BlsAMF3Serializer.cpp \
	BlsAMFTypes.cpp \
	BlsRtmpSource.cpp \
	BlsConsumer.cpp \
	BlsFlvRecoder.cpp \
	BlsFlashVideoInfo.cpp \
	BlsTimestampCorrector.cpp \
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
	BlsRtmpConnection.hpp \
	BlsRtmpUrl.hpp \
	BlsRtmpServer.hpp \
	BlsRtmpHandshake.hpp \
	BlsRtmpProtocol.hpp \
	BlsAMF0Serializer.hpp \
	BlsAMF3Serializer.hpp \
	BlsAMFTypes.hpp \
	BlsRtmpSource.hpp \
	BlsConsumer.hpp \
	BlsFlvRecoder.hpp \
	BlsFlashVideoInfo.hpp \
	BlsTimestampCorrector.hpp \
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
	@echo "build bls success."
	@echo -e "\\e[0m"
	mkdir -p ${top_builddir}/bin
	cp -f bls ${top_builddir}/bin
