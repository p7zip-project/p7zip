#
# makefile for Linux (x86, PPC, alpha ...)
#

# OPTFLAGS=-mfpmath=sse -Ofast -flto -march=native -funroll-loops -s
OPTFLAGS=-O -s

ALLFLAGS=${OPTFLAGS} -pipe -m32 \
    	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-DNDEBUG -D_REENTRANT -DENV_UNIX \
	-D_7ZIP_LARGE_PAGES \
	-D_7ZIP_ASM \
	$(LOCAL_FLAGS)

ALLFLAGS_CPP=-DENV_HAVE_GCCVISIBILITYPATCH -fvisibility=hidden -fvisibility-inlines-hidden

CXX=g++
CC=gcc
CC_SHARED=-fPIC
LINK_SHARED=-fPIC -shared
ASM=nasm -f elf

PRE_COMPILED_HEADER=StdAfx.h.gch

LOCAL_LIBS=-lpthread
LOCAL_LIBS_DLL=$(LOCAL_LIBS) -ldl

CPU=x86
OBJ_CRC32=$(OBJ_CRC32_x86)
OBJ_AES=AesOpt.o

