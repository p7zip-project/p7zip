# ##########################################################################
# LZ5 programs - Makefile
# Copyright (C) Yann Collet 2011-2015
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
#  - LZ5 source repository : https://github.com/inikep/lz5
#  - LZ5 public forum : https://groups.google.com/forum/#!forum/lz5c
# ##########################################################################
# lz5 : Command Line Utility, supporting gzip-like arguments
# lz5_32: Same as lz5, but forced to compile in 32-bits mode
# frametest  : Test tool, to check lz5frame integrity on target platform
# frametest32: Same as frametest, but forced to compile in 32-bits mode
# fullbench  : Precisely measure speed for each LZ5 function variant
# fullbench32: Same as fullbench, but forced to compile in 32-bits mode
# datagen : generates synthetic data samples for tests & benchmarks
# ##########################################################################

BINDIR  := $(PREFIX)/bin
MANDIR  := $(PREFIX)/share/man/man1
LZ5DIR  := ../lib

DESTDIR  ?=
PREFIX   ?= /usr/local
CFLAGS   ?= -O3 -std=gnu99    # can select customized optimized flags such as for example: make CFLAGS="-g"
FFLAGS   ?= -fomit-frame-pointer -fstrict-aliasing -fforce-addr -ffast-math
WFLAGS   ?= -Wall -Wextra -Wundef -Wshadow -Wcast-align -Wcast-qual -Wstrict-prototypes -pedantic -Wno-variadic-macros
CPPFLAGS := -I$(LZ5DIR) -DXXH_NAMESPACE=LZ5_ 
FLAGS    := $(CFLAGS) $(CPPFLAGS) $(WFLAGS) $(LDFLAGS) $(MOREFLAGS)


# Define *.exe as extension for Windows systems
ifneq (,$(filter Windows%,$(OS)))
EXT =.exe
VOID = nul
else
EXT =
VOID = /dev/null
endif


# Default test parameters
TEST_FILES   := COPYING
FUZZER_TIME  := -T9mn


default: lz5

m32: lz5_32 fullbench32 frametest32

bins: lz5 fullbench frametest datagen

all: bins m32

lz5: $(LZ5DIR)/lz5.o $(LZ5DIR)/lz5hc.o $(LZ5DIR)/lz5frame.o $(LZ5DIR)/xxhash.o bench.o lz5io.o lz5cli.o
	$(CC) $(FLAGS) $^ -o $@$(EXT)

lz5_32: $(LZ5DIR)/lz5.c $(LZ5DIR)/lz5hc.c $(LZ5DIR)/lz5frame.c $(LZ5DIR)/xxhash.c bench.c lz5io.c lz5cli.c
	$(CC) -m32 $(FLAGS) $^ -o $@$(EXT)

fullbench: $(LZ5DIR)/lz5.o $(LZ5DIR)/lz5hc.o $(LZ5DIR)/lz5frame.o $(LZ5DIR)/xxhash.o fullbench.o
	$(CC) $(FLAGS) $^ -o $@$(EXT)

fullbench32: $(LZ5DIR)/lz5.c $(LZ5DIR)/lz5hc.c $(LZ5DIR)/lz5frame.c $(LZ5DIR)/xxhash.c fullbench.c
	$(CC) -m32 $(FLAGS) $^ -o $@$(EXT)

frametest: $(LZ5DIR)/lz5frame.o $(LZ5DIR)/lz5.o $(LZ5DIR)/lz5hc.o $(LZ5DIR)/xxhash.o frametest.c
	$(CC) $(FLAGS) $^ -o $@$(EXT)

frametest32: $(LZ5DIR)/lz5frame.c $(LZ5DIR)/lz5.c $(LZ5DIR)/lz5hc.c $(LZ5DIR)/xxhash.c frametest.c
	$(CC) -m32 $(FLAGS) $^ -o $@$(EXT)

datagen : datagen.o datagencli.c
	$(CC) $(FLAGS) $^ -o $@$(EXT)

.c.o:
	$(CC) $(FLAGS) $< -c -o $@

clean:
	@$(MAKE) -C $(LZ5DIR) $@
	@rm -f core *.o *.test tmp* \
        lz5$(EXT) lz5_32$(EXT) \
        fullbench$(EXT) fullbench32$(EXT) \
        frametest$(EXT) frametest32$(EXT) \
        datagen$(EXT)
	@echo Cleaning completed


#------------------------------------------------------------------------
#make install is validated only for Linux, OSX, kFreeBSD and Hurd targets
ifneq (,$(filter $(shell uname),Linux Darwin GNU/kFreeBSD GNU MSYS_NT-10.0))

install: lz5 
	@echo Installing binaries
	@install -d -m 755 $(DESTDIR)$(BINDIR)/ $(DESTDIR)$(MANDIR)/
	@install -m 755 lz5 $(DESTDIR)$(BINDIR)/lz5
	@ln -sf lz5 $(DESTDIR)$(BINDIR)/lz5cat
	@ln -sf lz5 $(DESTDIR)$(BINDIR)/unlz5
	@echo Installing man pages
	@install -m 644 lz5.1 $(DESTDIR)$(MANDIR)/lz5.1
	@ln -sf lz5.1 $(DESTDIR)$(MANDIR)/lz5cat.1
	@ln -sf lz5.1 $(DESTDIR)$(MANDIR)/unlz5.1
	@echo lz5 installation completed

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/lz5cat
	rm -f $(DESTDIR)$(BINDIR)/unlz5
	[ -x $(DESTDIR)$(BINDIR)/lz5 ] && rm -f $(DESTDIR)$(BINDIR)/lz5
	[ -f $(DESTDIR)$(MANDIR)/lz5.1 ] && rm -f $(DESTDIR)$(MANDIR)/lz5.1
	rm -f $(DESTDIR)$(MANDIR)/lz5cat.1
	rm -f $(DESTDIR)$(MANDIR)/unlz5.1
	@echo lz5 programs successfully uninstalled

test: test-lz5 test-frametest test-fullbench test-mem

test32: test-lz5_32 test-frametest32 test-fullbench32 test-mem32

test-all: test test32

test-lz5-sparse: lz5 datagen
	@echo "\n ---- test sparse file support ----"
	./datagen -g5M  -P100 > tmpSrc
	./lz5 -B4 tmpSrc | ./lz5 -dv --sparse > tmpB4
	diff -s tmpSrc tmpB4
	./lz5 -B5 tmpSrc | ./lz5 -dv --sparse > tmpB5
	diff -s tmpSrc tmpB5
	./lz5 -B6 tmpSrc | ./lz5 -dv --sparse > tmpB6
	diff -s tmpSrc tmpB6
	./lz5 -B7 tmpSrc | ./lz5 -dv --sparse > tmpB7
	diff -s tmpSrc tmpB7
	./lz5 tmpSrc | ./lz5 -dv --no-sparse > tmpNoSparse
	diff -s tmpSrc tmpNoSparse
	ls -ls tmp*
	./datagen -s1 -g1200007 -P100 | ./lz5 | ./lz5 -dv --sparse > tmpOdd   # Odd size file (to generate non-full last block)
	./datagen -s1 -g1200007 -P100 | diff -s - tmpOdd
	ls -ls tmpOdd
	@rm tmp*
	@echo "\n Compatibility with Console :"
	echo "Hello World 1 !" | ./lz5 | ./lz5 -d -c
	echo "Hello World 2 !" | ./lz5 | ./lz5 -d | cat
	echo "Hello World 3 !" | ./lz5 --no-frame-crc | ./lz5 -d -c
	@echo "\n Compatibility with Append :"
	./datagen -P100 -g1M > tmp1M
	cat tmp1M tmp1M > tmp2M
	./lz5 -B5 -v tmp1M tmpC
	./lz5 -d -v tmpC tmpR
	./lz5 -d -v tmpC >> tmpR
	ls -ls tmp*
	diff tmp2M tmpR
	@rm tmp*

test-lz5-contentSize: lz5 datagen
	@echo "\n ---- test original size support ----"
	./datagen -g15M > tmp
	./lz5 -v tmp | ./lz5 -t
	./lz5 -v --content-size tmp | ./lz5 -d > tmp2
	diff -s tmp tmp2
	# test large size [2-4] GB
	@./datagen -g3G -P100 | ./lz5 | ./lz5 --decompress --force --sparse - tmp
	@ls -ls tmp
	./lz5 --quiet --content-size tmp | ./lz5 --verbose --decompress --force --sparse - tmp2
	@ls -ls tmp2
	@rm tmp*

test-lz5-frame-concatenation: lz5 datagen
	@echo "\n ---- test frame concatenation ----"
	@echo -n > empty.test
	@echo hi > nonempty.test
	cat nonempty.test empty.test nonempty.test > orig.test
	@./lz5 -zq empty.test > empty.lz5.test
	@./lz5 -zq nonempty.test > nonempty.lz5.test
	cat nonempty.lz5.test empty.lz5.test nonempty.lz5.test > concat.lz5.test
	./lz5 -d concat.lz5.test > result.test
	sdiff orig.test result.test
	@rm *.test
	@echo frame concatenation test completed

test-lz5-multiple: lz5 datagen
	@echo "\n ---- test multiple files ----"
	@./datagen -s1        > tmp1 2> $(VOID)
	@./datagen -s2 -g100K > tmp2 2> $(VOID)
	@./datagen -s3 -g1M   > tmp3 2> $(VOID)
	./lz5 -f -m tmp*
	ls -ls tmp*
	rm tmp1 tmp2 tmp3
	./lz5 -df -m *.lz5
	ls -ls tmp*
	./lz5 -f -m tmp1 notHere tmp2; echo $$?
	@rm tmp*

test-lz5-basic: lz5 datagen
	@echo "\n ---- test lz5 basic compression/decompression ----"
	./datagen -g0     | ./lz5 -v     | ./lz5 -t
	./datagen -g16KB  | ./lz5 -9     | ./lz5 -t
	./datagen -g20KB > tmpSrc
	./lz5 < tmpSrc | ./lz5 -d > tmpRes
	diff -q tmpSrc tmpRes
	./lz5 --no-frame-crc < tmpSrc | ./lz5 -d > tmpRes
	diff -q tmpSrc tmpRes
	./datagen         | ./lz5        | ./lz5 -t
	./datagen -g6M -P99 | ./lz5 -9B | ./lz5 -t
	./datagen -g17M   | ./lz5 -9v    | ./lz5 -qt
	./datagen -g33M   | ./lz5 --no-frame-crc | ./lz5 -t
	./datagen -g256MB | ./lz5 -vqB4 | ./lz5 -t
	./datagen -g6GB   | ./lz5 -vqB5 | ./lz5 -qt
	./datagen -g6GB   | ./lz5 -vq9B | ./lz5 -qt
	@rm tmp*

test-lz5: lz5 datagen test-lz5-basic test-lz5-multiple test-lz5-sparse test-lz5-contentSize test-lz5-frame-concatenation
	@echo "\n ---- test pass-through ----"
	./datagen | ./lz5 -tf

test-interop-32-64: lz5 lz5_32 datagen
	@echo "\n ---- test interoperability 32-bits -vs- 64 bits ----"
	./datagen -g16KB  | ./lz5_32 -9     | ./lz5    -t
	./datagen -P10    | ./lz5    -9B4   | ./lz5_32 -t
	./datagen         | ./lz5_32        | ./lz5    -t
	./datagen -g1M    | ./lz5    -3B5   | ./lz5_32 -t
	./datagen -g256MB | ./lz5_32 -vqB4  | ./lz5    -qt
	./datagen -g1G -P90 | ./lz5         | ./lz5_32 -t
	./datagen -g6GB   | ./lz5_32 -vq9B  | ./lz5    -qt

test-lz5_32-basic: lz5_32 datagen
	@echo "\n ---- test lz5_32 32-bits version ----"
	./datagen -g16KB  | ./lz5_32 -9     | ./lz5_32 -t
	./datagen         | ./lz5_32        | ./lz5_32 -t
	./datagen -g256MB | ./lz5_32 -vqB4  | ./lz5_32 -qt
	./datagen -g6GB   | ./lz5_32 -vqB5  | ./lz5_32 -qt

test-lz5_32: test-lz5_32-basic test-interop-32-64

test-fullbench: fullbench
	./fullbench --no-prompt $(NB_LOOPS) $(TEST_FILES)

test-fullbench32: fullbench32
	./fullbench32 --no-prompt $(NB_LOOPS) $(TEST_FILES)

test-fuzzer: fuzzer
	./fuzzer $(FUZZER_TIME)

test-fuzzer32: fuzzer32
	./fuzzer32 $(FUZZER_TIME)

test-frametest-gdb: frametest
	echo >gdb_cmd run $(FUZZER_TIME) && echo >>gdb_cmd bt
	gdb --batch --command ./gdb_cmd ./frametest

test-frametest: frametest
	./frametest $(FUZZER_TIME)

test-frametest32: frametest32
	./frametest32 $(FUZZER_TIME)

test-mem: lz5 datagen frametest fullbench
	@echo "\n ---- valgrind tests : memory analyzer ----"
	valgrind --leak-check=yes --error-exitcode=1 ./datagen -g50M > $(VOID)
	./datagen -g16KB > tmp
	valgrind --leak-check=yes --error-exitcode=1 ./lz5 -9 -B -f tmp $(VOID)
	./datagen -g16KB -s2 > tmp2
	./datagen -g16KB -s3 > tmp3
	valgrind --leak-check=yes --error-exitcode=1 ./lz5 --force --multiple tmp tmp2 tmp3
	./datagen -g16MB > tmp
	valgrind --leak-check=yes --error-exitcode=1 ./lz5 -9 -B5 -f tmp tmp2
	valgrind --leak-check=yes --error-exitcode=1 ./lz5 -t tmp2
	valgrind --leak-check=yes --error-exitcode=1 ./lz5 -bi1 tmp
	valgrind --leak-check=yes --error-exitcode=1 ./fullbench -i1 tmp tmp2
	./datagen -g256MB > tmp
	valgrind --leak-check=yes --error-exitcode=1 ./lz5 -B4 -f -vq tmp $(VOID)
	rm tmp*
	valgrind --leak-check=yes --error-exitcode=1 ./frametest -i256

test-mem32: lz5_32 datagen
# unfortunately, valgrind doesn't seem to work with non-native binary...

endif
