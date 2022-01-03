
TARGET_FLAGS=$(CC_SHARED) -DRegisterCodec=DllRegisterCodec

MY_WINDOWS=

include ../../../../makefile.crc32
include ../../../../makefile.machine

LOCAL_LINK=$(LINK_SHARED)
LIBS=$(LOCAL_LIBS)

COMPRESS_OBJS = \
  CodecExports.o \
  DllExportsCompress.o \

OPT_OBJS = \
  PKImplodeDecoder.o \
  PKImplodeRegister.o \

COMMON_OBJS = 

P7ZIP_COMMON_OBJS = \
  InBuffer.o \
  OutBuffer.o \
  StreamUtils.o \

C_OBJS = \
  blast/blast.o \

OBJS = \
MyWindows.o \
$(COMPRESS_OBJS) \
$(OPT_OBJS) \
$(COMMON_OBJS) \
$(P7ZIP_COMMON_OBJS) \
$(C_OBJS) \

include ../../../../makefile.glb

