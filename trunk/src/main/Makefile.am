INCLUDES = -I@TP_INCLUDE@ -I@TOP_INCLUDE@
bin_PROGRAMS = bls

bls_LDFLAGS = -ldl
bls_LDFLAGS += -lz
bls_LDADD  = ../mrtmp/libmrtmp.a
bls_LDADD += ../mnetwork/libmnetwork.a
bls_LDADD += ../mcore/libmcore.a
bls_LDADD += @TP_LIBS@
bls_SOURCES = main.cpp

all-am:
	@echo -e "\\e[32m"
	@echo "build bls success."
	@echo -e "\\e[0m"
