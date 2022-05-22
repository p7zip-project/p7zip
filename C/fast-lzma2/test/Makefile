OBJ=file_test.o
LIB:=-lfast-lzma2	

CFLAGS:=-Wall -O1 -pthread
CC:=gcc

EXT:=
ifeq ($(OS),Windows_NT)
	EXT:=.exe
	LIB:=../libfast-lzma2.dll
	CFLAGS+=-I..
endif

file_test : $(OBJ)
	$(CC) -pthread -o file_test$(EXT) $(OBJ) $(LIB)

clean:
	rm -f file_test$(EXT) $(OBJ)
