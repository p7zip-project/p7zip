# ################################################################
# Lizard - Makefile
# Copyright (C) 2011-2015, Yann Collet
# Copyright (C) 2016-2017, Przemyslaw Skibinski <inikep@gmail.com>
# All rights reserved.
#
# This Makefile is validated for Linux, macOS, *BSD, Hurd, Solaris, MSYS2 targets
#
# BSD license
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# You can contact the author at :
#  - Lizard source repository : https://github.com/inikep/lizard
# ################################################################

DESTDIR ?=
PREFIX  ?= /usr/local
VOID    := /dev/null

PRGDIR  = programs
LIBDIR  = lib
TESTDIR = tests


# Define nul output
ifneq (,$(filter Windows%,$(OS)))
EXT = .exe
else
EXT =
endif


.PHONY: default all lib lizard clean test versionsTest examples

default: lizard

all: lib lizard

lib:
	@$(MAKE) -C $(LIBDIR) all

lizard:
	@$(MAKE) -C $(PRGDIR)
	@cp $(PRGDIR)/lizard$(EXT) .

clean:
	@$(MAKE) -C $(PRGDIR) $@ > $(VOID)
	@$(MAKE) -C $(TESTDIR) $@ > $(VOID)
	@$(MAKE) -C $(LIBDIR) $@ > $(VOID)
	@$(MAKE) -C examples $@ > $(VOID)
	@$(RM) lizard$(EXT)
	@echo Cleaning completed


#-----------------------------------------------------------------------------
# make install is validated only for Linux, OSX, BSD, Hurd, Haiku and Solaris targets
#-----------------------------------------------------------------------------
ifneq (,$(filter $(shell uname),Linux Darwin GNU/kFreeBSD GNU OpenBSD FreeBSD NetBSD DragonFly SunOS Haiku))
HOST_OS = POSIX

install:
	@$(MAKE) -C $(LIBDIR) $@
	@$(MAKE) -C $(PRGDIR) $@

uninstall:
	@$(MAKE) -C $(LIBDIR) $@
	@$(MAKE) -C $(PRGDIR) $@

travis-install:
	$(MAKE) -j1 install PREFIX=~/install_test_dir

test:
	$(MAKE) -C $(TESTDIR) $@

clangtest: clean
	clang -v
	@CFLAGS="-O3 -Werror -Wconversion -Wno-sign-conversion" $(MAKE) -C $(LIBDIR)  all CC=clang
	@CFLAGS="-O3 -Werror -Wconversion -Wno-sign-conversion" $(MAKE) -C $(PRGDIR)  all CC=clang
	@CFLAGS="-O3 -Werror -Wconversion -Wno-sign-conversion" $(MAKE) -C $(TESTDIR) all CC=clang

clangtest-native: clean
	clang -v
	@CFLAGS="-O3 -Werror -Wconversion -Wno-sign-conversion" $(MAKE) -C $(LIBDIR)  all    CC=clang
	@CFLAGS="-O3 -Werror -Wconversion -Wno-sign-conversion" $(MAKE) -C $(PRGDIR)  native CC=clang
	@CFLAGS="-O3 -Werror -Wconversion -Wno-sign-conversion" $(MAKE) -C $(TESTDIR) native CC=clang

sanitize: clean
	CFLAGS="-O3 -g -DLIZARD_NO_HUFFMAN -fsanitize=undefined" $(MAKE) test CC=clang FUZZER_TIME="-T1mn" NB_LOOPS=-i1

staticAnalyze: clean
	CFLAGS=-g scan-build --status-bugs -v $(MAKE) all

platformTest: clean
	@echo "\n ---- test lizard with $(CC) compiler ----"
	@$(CC) -v
	CFLAGS="-O3 -Werror"         $(MAKE) -C $(LIBDIR) all
	CFLAGS="-O3 -Werror -static" $(MAKE) -C $(PRGDIR) native
	CFLAGS="-O3 -Werror -static" $(MAKE) -C $(TESTDIR) native
	$(MAKE) -C $(TESTDIR) test-platform

versionsTest: clean
	$(MAKE) -C $(TESTDIR) $@

examples:
	$(MAKE) -C $(LIBDIR)
	$(MAKE) -C $(PRGDIR) lizard
	$(MAKE) -C examples test

endif


ifneq (,$(filter MSYS%,$(shell uname)))
HOST_OS = MSYS
CMAKE_PARAMS = -G"MSYS Makefiles"
endif


#------------------------------------------------------------------------
#make tests validated only for MSYS, Linux, OSX, kFreeBSD and Hurd targets
#------------------------------------------------------------------------
ifneq (,$(filter $(HOST_OS),MSYS POSIX))

cmake:
	@cd contrib/cmake_unofficial; cmake $(CMAKE_PARAMS) CMakeLists.txt; $(MAKE)

gpptest: clean
	g++ -v
	CC=g++ $(MAKE) -C $(LIBDIR)  all CFLAGS="-O3 -Wall -Wextra -Wundef -Wshadow -Wcast-align -Werror"
	CC=g++ $(MAKE) -C $(PRGDIR)  all CFLAGS="-O3 -Wall -Wextra -Wundef -Wshadow -Wcast-align -Werror"
	CC=g++ $(MAKE) -C $(TESTDIR) all CFLAGS="-O3 -Wall -Wextra -Wundef -Wshadow -Wcast-align -Werror"

gpptest-native: clean
	g++ -v
	CC=g++ $(MAKE) -C $(LIBDIR)  all    CFLAGS="-O3 -Wall -Wextra -Wundef -Wshadow -Wcast-align -Werror"
	CC=g++ $(MAKE) -C $(PRGDIR)  native CFLAGS="-O3 -Wall -Wextra -Wundef -Wshadow -Wcast-align -Werror"
	CC=g++ $(MAKE) -C $(TESTDIR) native CFLAGS="-O3 -Wall -Wextra -Wundef -Wshadow -Wcast-align -Werror"

c_standards: clean
	$(MAKE) all MOREFLAGS="-std=gnu90 -Werror"
	$(MAKE) clean
	$(MAKE) all MOREFLAGS="-std=c99 -Werror"
	$(MAKE) clean
	$(MAKE) all MOREFLAGS="-std=gnu99 -Werror"
	$(MAKE) clean
	$(MAKE) all MOREFLAGS="-std=c11 -Werror"
	$(MAKE) clean

endif
