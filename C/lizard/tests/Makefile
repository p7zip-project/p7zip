# ##########################################################################
# Lizard programs - Makefile
# Copyright (C) Yann Collet 2011-2016
# Copyright (C) Przemyslaw Skibinski 2016-2017
#
# GPL v2 License
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# You can contact the author at :
#  - Lizard source repository : https://github.com/inikep/lizard
# ##########################################################################
# fuzzer  : Test tool, to check lizard integrity on target platform
# fuzzer32: Same as fuzzer, but forced to compile in 32-bits mode
# frametest  : Test tool, to check lizard_frame integrity on target platform
# frametest32: Same as frametest, but forced to compile in 32-bits mode
# fullbench  : Precisely measure speed for each Lizard function variant
# fullbench32: Same as fullbench, but forced to compile in 32-bits mode
# datagen : generates synthetic data samples for tests & benchmarks
# ##########################################################################

DESTDIR ?=
PREFIX  ?= /usr/local
VOID    := /dev/null
BINDIR  := $(PREFIX)/bin
MANDIR  := $(PREFIX)/share/man/man1
LIBDIR  := ../lib
PRGDIR  := ../programs
TESTDIR := versionsTest
PYTHON  ?= python3
ENTROPY_FILES := $(LIBDIR)/entropy/*.c

CFLAGS  ?= -O3   # can select custom flags. For example : CFLAGS="-O2 -g" make
CFLAGS  += -Wall -Wextra -Wundef -Wcast-qual -Wcast-align -Wshadow -Wswitch-enum \
           -Wdeclaration-after-statement -Wstrict-prototypes \
           -Wpointer-arith -Wstrict-aliasing=1
CFLAGS  += $(MOREFLAGS)
CPPFLAGS:= -I$(LIBDIR) -DXXH_NAMESPACE=Lizard_
FLAGS   := $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)


# Define *.exe as extension for Windows systems
ifneq (,$(filter Windows%,$(OS)))
EXT =.exe
else
EXT =
endif


# Default test parameters
TEST_FILES   := COPYING
FUZZER_TIME  := -T3mn
NB_LOOPS     ?= -i1


default: native

m32: fullbench32 fuzzer32 frametest32

native: fullbench fuzzer frametest datagen

all: native m32


lizard:
	$(MAKE) -C $(PRGDIR) $@

lizardc:
	$(MAKE) -C $(PRGDIR) $@

lizardc32:
	$(MAKE) -C $(PRGDIR) $@

fullbench  : $(LIBDIR)/lizard_compress.o $(LIBDIR)/lizard_decompress.o $(LIBDIR)/lizard_frame.o $(LIBDIR)/xxhash/xxhash.o fullbench.c $(ENTROPY_FILES)
	$(CC) $(FLAGS) $^ -o $@$(EXT)

fullbench32: $(LIBDIR)/lizard_compress.c $(LIBDIR)/lizard_decompress.c $(LIBDIR)/lizard_frame.c $(LIBDIR)/xxhash/xxhash.c fullbench.c $(ENTROPY_FILES)
	$(CC) -m32 $(FLAGS) $^ -o $@$(EXT)

fullbench-lib: fullbench.c $(LIBDIR)/xxhash/xxhash.c
	$(MAKE) -C $(LIBDIR) liblizard.a
	$(CC) $(FLAGS) $^ -o $@$(EXT) $(LIBDIR)/liblizard.a

fullbench-dll: fullbench.c $(LIBDIR)/xxhash/xxhash.c
	$(MAKE) -C $(LIBDIR) liblizard
	$(CC) $(FLAGS) $^ -o $@$(EXT) -DLIZARD_DLL_IMPORT=1 $(LIBDIR)/dll/liblizard.dll

fuzzer  : $(LIBDIR)/lizard_compress.c $(LIBDIR)/lizard_decompress.o $(LIBDIR)/xxhash/xxhash.o fuzzer.c $(ENTROPY_FILES)
	$(CC) -DLIZARD_RESET_MEM $(FLAGS) $^ -o $@$(EXT)

fuzzer32: $(LIBDIR)/lizard_compress.c $(LIBDIR)/lizard_decompress.c $(LIBDIR)/xxhash/xxhash.c fuzzer.c $(ENTROPY_FILES)
	$(CC) -m32 -DLIZARD_RESET_MEM $(FLAGS) $^ -o $@$(EXT)

frametest: $(LIBDIR)/lizard_frame.o $(LIBDIR)/lizard_compress.o $(LIBDIR)/lizard_decompress.o $(LIBDIR)/xxhash/xxhash.o frametest.c $(ENTROPY_FILES)
	$(CC) $(FLAGS) $^ -o $@$(EXT)

frametest32: $(LIBDIR)/lizard_frame.c $(LIBDIR)/lizard_compress.c $(LIBDIR)/lizard_decompress.c $(LIBDIR)/xxhash/xxhash.c frametest.c $(ENTROPY_FILES)
	$(CC) -m32 $(FLAGS) $^ -o $@$(EXT)

datagen : $(PRGDIR)/datagen.c datagencli.c
	$(CC) $(FLAGS) -I$(PRGDIR) $^ -o $@$(EXT)

clean:
	@$(MAKE) -C $(LIBDIR) $@ > $(VOID)
	@$(RM) core *.o *.test tmp* \
        fullbench-dll$(EXT) fullbench-lib$(EXT) \
        fullbench$(EXT) fullbench32$(EXT) \
        fuzzer$(EXT) fuzzer32$(EXT) \
        frametest$(EXT) frametest32$(EXT) \
        datagen$(EXT)
	@rm -fR $(TESTDIR)
	@echo Cleaning completed

versionsTest:
	$(PYTHON) test-lizard-versions.py


#------------------------------------------------------------------------
#make install is validated only for Linux, OSX, kFreeBSD, Hurd, Haiku and
#FreeBSD targets
ifneq (,$(filter $(shell uname),Linux Darwin GNU/kFreeBSD GNU FreeBSD Haiku))

test: test-lizard test-lizardc test-frametest test-fullbench test-fuzzer

test32: test-lizardc32 test-frametest32 test-fullbench32 test-fuzzer32

test-all: test test32

test-lizard-sparse: lizard datagen
	@echo "\n ---- test sparse file support ----"
	./datagen -g5M  -P100 > tmpSrc
	$(PRGDIR)/lizard -B4D tmpSrc | $(PRGDIR)/lizard -dv --sparse > tmpB4
	diff -s tmpSrc tmpB4
	$(PRGDIR)/lizard -B5D tmpSrc | $(PRGDIR)/lizard -dv --sparse > tmpB5
	diff -s tmpSrc tmpB5
	$(PRGDIR)/lizard -B6D tmpSrc | $(PRGDIR)/lizard -dv --sparse > tmpB6
	diff -s tmpSrc tmpB6
	$(PRGDIR)/lizard -B7D tmpSrc | $(PRGDIR)/lizard -dv --sparse > tmpB7
	diff -s tmpSrc tmpB7
	$(PRGDIR)/lizard tmpSrc | $(PRGDIR)/lizard -dv --no-sparse > tmpNoSparse
	diff -s tmpSrc tmpNoSparse
	ls -ls tmp*
	./datagen -s1 -g1200007 -P100 | $(PRGDIR)/lizard | $(PRGDIR)/lizard -dv --sparse > tmpOdd   # Odd size file (to generate non-full last block)
	./datagen -s1 -g1200007 -P100 | diff -s - tmpOdd
	ls -ls tmpOdd
	@$(RM) tmp*
	@echo "\n Compatibility with Console :"
	echo "Hello World 1 !" | $(PRGDIR)/lizard | $(PRGDIR)/lizard -d -c
	echo "Hello World 2 !" | $(PRGDIR)/lizard | $(PRGDIR)/lizard -d | cat
	echo "Hello World 3 !" | $(PRGDIR)/lizard --no-frame-crc | $(PRGDIR)/lizard -d -c
	@echo "\n Compatibility with Append :"
	./datagen -P100 -g1M > tmp1M
	cat tmp1M tmp1M > tmp2M
	$(PRGDIR)/lizard -B5 -v tmp1M tmpC
	$(PRGDIR)/lizard -d -v tmpC tmpR
	$(PRGDIR)/lizard -d -v tmpC >> tmpR
	ls -ls tmp*
	diff tmp2M tmpR
	@$(RM) tmp*

test-lizard-contentSize: lizard datagen
	@echo "\n ---- test original size support ----"
	./datagen -g15M > tmp
	$(PRGDIR)/lizard -v tmp | $(PRGDIR)/lizard -t
	$(PRGDIR)/lizard -v --content-size tmp | $(PRGDIR)/lizard -d > tmp2
	diff -s tmp tmp2
	# test large size [2-4] GB
	@./datagen -g3G -P100 | $(PRGDIR)/lizard | $(PRGDIR)/lizard --decompress --force --sparse - tmp
	@ls -ls tmp
	$(PRGDIR)/lizard --quiet --content-size tmp | $(PRGDIR)/lizard --verbose --decompress --force --sparse - tmp2
	@ls -ls tmp2
	@$(RM) tmp*

test-lizard-frame-concatenation: lizard datagen
	@echo "\n ---- test frame concatenation ----"
	@echo -n > empty.test
	@echo hi > nonempty.test
	cat nonempty.test empty.test nonempty.test > orig.test
	@$(PRGDIR)/lizard -zq empty.test > empty.liz.test
	@$(PRGDIR)/lizard -zq nonempty.test > nonempty.liz.test
	cat nonempty.liz.test empty.liz.test nonempty.liz.test > concat.liz.test
	$(PRGDIR)/lizard -d concat.liz.test > result.test
	sdiff orig.test result.test
	@$(RM) *.test
	@echo frame concatenation test completed

test-lizard-multiple: lizard datagen
	@echo "\n ---- test multiple files ----"
	@./datagen -s1        > tmp1 2> $(VOID)
	@./datagen -s2 -g100K > tmp2 2> $(VOID)
	@./datagen -s3 -g1M   > tmp3 2> $(VOID)
	$(PRGDIR)/lizard -f -m tmp*
	ls -ls tmp*
	@$(RM) tmp1 tmp2 tmp3
	$(PRGDIR)/lizard -df -m *.liz
	ls -ls tmp*
	$(PRGDIR)/lizard -f -m tmp1 notHere tmp2; echo $$?
	@$(RM) tmp*

test-lizard-basic: lizard datagen
	@echo "\n ---- test lizard basic compression/decompression ----"
	./datagen -g0     | $(PRGDIR)/lizard -v     | $(PRGDIR)/lizard -t
	./datagen -g16KB  | $(PRGDIR)/lizard -19    | $(PRGDIR)/lizard -t
	./datagen -g20KB > tmpSrc
	$(PRGDIR)/lizard < tmpSrc | $(PRGDIR)/lizard -d > tmpRes
	diff -q tmpSrc tmpRes
	$(PRGDIR)/lizard --no-frame-crc < tmpSrc | $(PRGDIR)/lizard -d > tmpRes
	diff -q tmpSrc tmpRes
	./datagen         | $(PRGDIR)/lizard        | $(PRGDIR)/lizard -t
	./datagen -g6M -P99 | $(PRGDIR)/lizard -19BD | $(PRGDIR)/lizard -t
	./datagen -g17M   | $(PRGDIR)/lizard -19v    | $(PRGDIR)/lizard -qt
	./datagen -g33M   | $(PRGDIR)/lizard --no-frame-crc | $(PRGDIR)/lizard -t
	./datagen -g256MB | $(PRGDIR)/lizard -vqB4D | $(PRGDIR)/lizard -t
	@$(RM) tmp*

test-lizard-hugefile: lizard datagen
	@echo "\n ---- test huge files compression/decompression ----"
	./datagen -g6GB   | $(PRGDIR)/lizard -vB5D  | $(PRGDIR)/lizard -qt
	./datagen -g6GB   | $(PRGDIR)/lizard -v15BD  | $(PRGDIR)/lizard -qt
	@$(RM) tmp*

test-lizard-testmode: lizard datagen
	@echo "\n ---- bench mode ----"
	$(PRGDIR)/lizard -bi1
	@echo "\n ---- test mode ----"
	./datagen | $(PRGDIR)/lizard -t             && false || true
	./datagen | $(PRGDIR)/lizard -tf            && false || true
	@echo "\n ---- pass-through mode ----"
	./datagen | $(PRGDIR)/lizard -d  > $(VOID)  && false || true
	./datagen | $(PRGDIR)/lizard -df > $(VOID)
	@echo "Hello World !" > tmp1
	$(PRGDIR)/lizard -dcf tmp1
	@echo "from underground..." > tmp2
	$(PRGDIR)/lizard -dcfm tmp1 tmp2
	@echo "\n ---- test cli ----"
	$(PRGDIR)/lizard     file-does-not-exist    && false || true
	$(PRGDIR)/lizard -f  file-does-not-exist    && false || true
	$(PRGDIR)/lizard -fm file1-dne file2-dne    && false || true
	$(PRGDIR)/lizard -fm file1-dne file2-dne    && false || true

test-lizard: lizard datagen test-lizard-basic test-lizard-multiple test-lizard-sparse \
          test-lizard-frame-concatenation test-lizard-testmode test-lizard-contentSize \
          test-lizard-hugefile

test-lizardc: lizardc datagen
	@echo "\n ---- test lizardc version ----"
	./datagen -g256MB | $(PRGDIR)/lizardc -l -v    | $(PRGDIR)/lizardc   -t

test-interop-32-64: lizard lizardc32 datagen
	@echo "\n ---- test interoperability 32-bits -vs- 64 bits ----"
	./datagen -g16KB  | $(PRGDIR)/lizardc32 -19    | $(PRGDIR)/lizard    -t
	./datagen -P10    | $(PRGDIR)/lizard    -19B4  | $(PRGDIR)/lizardc32 -t
	./datagen         | $(PRGDIR)/lizardc32        | $(PRGDIR)/lizard    -t
	./datagen -g1M    | $(PRGDIR)/lizard    -13B5  | $(PRGDIR)/lizardc32 -t
	./datagen -g256MB | $(PRGDIR)/lizardc32 -vqB4D | $(PRGDIR)/lizard    -qt
	./datagen -g1G -P90 | $(PRGDIR)/lizard         | $(PRGDIR)/lizardc32 -t
	./datagen -g6GB   | $(PRGDIR)/lizardc32 -vq15BD | $(PRGDIR)/lizard    -qt

test-lizardc32-basic: lizardc32 datagen
	@echo "\n ---- test lizardc32 32-bits version ----"
	./datagen -g16KB  | $(PRGDIR)/lizardc32 -19    | $(PRGDIR)/lizardc32 -t
	./datagen         | $(PRGDIR)/lizardc32        | $(PRGDIR)/lizardc32 -t
	./datagen -g256MB | $(PRGDIR)/lizardc32 -vqB4D | $(PRGDIR)/lizardc32 -qt
	./datagen -g6GB   | $(PRGDIR)/lizardc32 -vqB5D | $(PRGDIR)/lizardc32 -qt

test-platform:
	@echo "\n ---- test lizard $(QEMU_SYS) platform ----"
	$(QEMU_SYS) ./datagen -g16KB  | $(QEMU_SYS) $(PRGDIR)/lizard -19    | $(QEMU_SYS) $(PRGDIR)/lizard -t
	$(QEMU_SYS) ./datagen         | $(QEMU_SYS) $(PRGDIR)/lizard        | $(QEMU_SYS) $(PRGDIR)/lizard -t
	$(QEMU_SYS) ./datagen -g256MB | $(QEMU_SYS) $(PRGDIR)/lizard -vqB4D | $(QEMU_SYS) $(PRGDIR)/lizard -qt
ifneq ($(QEMU_SYS),qemu-arm-static)
ifneq ($(QEMU_SYS),qemu-ppc-static)
	$(QEMU_SYS) ./datagen -g3GB   | $(QEMU_SYS) $(PRGDIR)/lizard -vqB5D | $(QEMU_SYS) $(PRGDIR)/lizard -qt
endif
endif

test-lizardc32: test-lizardc32-basic test-interop-32-64

test-fullbench: fullbench
	./fullbench --no-prompt $(NB_LOOPS) $(TEST_FILES)

test-fullbench32: fullbench32
	./fullbench32 --no-prompt $(NB_LOOPS) $(TEST_FILES)

test-fuzzer: fuzzer
	./fuzzer $(FUZZER_TIME)

test-fuzzer32: fuzzer32
	./fuzzer32 $(FUZZER_TIME)

test-frametest: frametest
	./frametest $(FUZZER_TIME)

test-frametest32: frametest32
	./frametest32 $(FUZZER_TIME)

test-mem: lizard datagen fuzzer frametest fullbench
	@echo "\n ---- valgrind tests : memory analyzer ----"
	valgrind --leak-check=yes --error-exitcode=1 ./datagen -g50M > $(VOID)
	./datagen -g16KB > tmp
	valgrind --leak-check=yes --error-exitcode=1 $(PRGDIR)/lizard -19 -BD -f tmp $(VOID)
	./datagen -g16KB -s2 > tmp2
	./datagen -g16KB -s3 > tmp3
	valgrind --leak-check=yes --error-exitcode=1 $(PRGDIR)/lizard --force --multiple tmp tmp2 tmp3
	./datagen -g16MB > tmp
	valgrind --leak-check=yes --error-exitcode=1 $(PRGDIR)/lizard -19 -B5D -f tmp tmp2
	valgrind --leak-check=yes --error-exitcode=1 $(PRGDIR)/lizard -t tmp2
	valgrind --leak-check=yes --error-exitcode=1 $(PRGDIR)/lizard -bi1 tmp
	valgrind --leak-check=yes --error-exitcode=1 ./fullbench -i1 tmp tmp2
	./datagen -g256MB > tmp
	valgrind --leak-check=yes --error-exitcode=1 $(PRGDIR)/lizard -B4D -f -vq tmp $(VOID)
	$(RM) tmp*
	valgrind --leak-check=yes --error-exitcode=1 ./fuzzer -i64 -t1
	valgrind --leak-check=yes --error-exitcode=1 ./frametest -i256

test-mem32: lizardc32 datagen
# unfortunately, valgrind doesn't seem to work with non-native binary...

endif
