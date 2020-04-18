#
# makefile for NetWare cross-gcc (x86)
#

CROSSPREFIX=i586-netware-
BINSUFFIX=.nlm

OPTFLAGS=-O -s

ALLFLAGS=${OPTFLAGS} \
	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-DNDEBUG -D_REENTRANT -DENV_UNIX -D__NETWARE__ -D_POSIX_SOURCE \
	-fpcc-struct-return \
	-D_7ZIP_ASM \
	$(LOCAL_FLAGS)

CXX=$(CROSSPREFIX)g++
CC=$(CROSSPREFIX)gcc
# LINK_SHARED=-shared
ASM=nasm -f elf
AWK=gawk

LOCAL_LIBS=
LOCAL_LIBS_DLL=$(LOCAL_LIBS)

CPU=x86
OBJ_CRC32=$(OBJ_CRC32_x86)
OBJ_AES=AesOpt.o

VERSION_INPUT=../../../7zip/MyVersion.h
P7ZIP_VERSION_AWK='/^\#define MY_VERSION /{gsub("\"","",$$3);print $$3}'
P7ZIP_VERSION=$(shell $(AWK) $(P7ZIP_VERSION_AWK) $(VERSION_INPUT))
P7ZIP_COPYRIGHT_AWK='/^\#define MY_COPYRIGHT/{gsub($$1" "$$2" ","",$$0);print $$0}'
P7ZIP_COPYRIGHT=$(shell $(AWK) $(P7ZIP_COPYRIGHT_AWK) $(VERSION_INPUT))

LDFLAGS=-Wl,--nlm-description="7-Zip $(P7ZIP_VERSION) File Archiver (gcc build)"
LDFLAGS+=-Wl,--nlm-copyright=$(P7ZIP_COPYRIGHT)
LDFLAGS+=-Wl,--nlm-version=$(P7ZIP_VERSION)
LDFLAGS+=-Wl,--nlm-kernelspace
# LDFLAGS+=-Wl,--nlm-posixflag
LDFLAGS+=-Wl,--nlm-flag-off=2


