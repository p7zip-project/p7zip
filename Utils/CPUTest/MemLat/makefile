PROG=MemLat


# ./MemLat   4 p5 n

# -DLARGE_PAGES
LOCAL_FLAGS=-I. -DNUM_DIRECT_TESTS=6 -DNUM_PAR_TESTS=5

include ../../../makefile.machine

TARGET_FLAGS=$(CC_EXE)
LOCAL_LINK=$(LINK_EXE)
LIBS=$(LOCAL_LIBS)

OBJS=MemLat.o Walk.o

include ../../../makefile.glb

