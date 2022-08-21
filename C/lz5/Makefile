# ################################################################
# LZ5 - Makefile
# Copyright (C) Yann Collet 2011-2015
# All rights reserved.
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
#  - LZ5 source repository : https://github.com/inikep/lz5
#  - LZ5 forum froup : https://groups.google.com/forum/#!forum/lz5c
# ################################################################

DESTDIR?=
PREFIX ?= /usr/local

LIBDIR ?= $(PREFIX)/lib
INCLUDEDIR=$(PREFIX)/include
PRGDIR  = programs
LZ5DIR  = lib


# Define nul output
ifneq (,$(filter Windows%,$(OS)))
VOID = nul
else
VOID = /dev/null
endif


.PHONY: default all lib lz5programs clean test versionsTest examples

default: lz5programs

all: lib lz5programs

lib:
	@$(MAKE) -C $(LZ5DIR) all

lz5programs:
	@$(MAKE) -C $(PRGDIR)

clean:
	@$(MAKE) -C $(PRGDIR) $@ > $(VOID)
	@$(MAKE) -C $(LZ5DIR) $@ > $(VOID)
	@echo Cleaning completed


#------------------------------------------------------------------------
#make install is validated only for Linux, OSX, kFreeBSD and Hurd targets
ifneq (,$(filter $(shell uname),Linux Darwin GNU/kFreeBSD GNU))

install:
	@$(MAKE) -C $(LZ5DIR) $@
	@$(MAKE) -C $(PRGDIR) $@

uninstall:
	@$(MAKE) -C $(LZ5DIR) $@
	@$(MAKE) -C $(PRGDIR) $@

test:
	$(MAKE) -C $(PRGDIR) test

gpptest: clean
	$(MAKE) all CC=g++ CFLAGS="-O3" FFLAGS="" WFLAGS="-Wall -Wextra -Wundef -Wshadow -Wcast-align -Wcast-qual -Wno-variadic-macros -Werror"

clangtest: clean
	$(MAKE) all CC=clang MOREFLAGS="-Werror -Wconversion -Wno-sign-conversion"

sanitize: clean
	$(MAKE) test CC=clang MOREFLAGS="-g -fsanitize=undefined -DLZ5_RESET_MEM" FUZZER_TIME="-T1mn" NB_LOOPS=-i1

staticAnalyze: clean
	MOREFLAGS="-g" scan-build --status-bugs -v $(MAKE) all

armtest: clean
	$(MAKE) -C $(LZ5DIR) all CC=arm-linux-gnueabi-gcc MOREFLAGS="-Werror"
	$(MAKE) -C $(PRGDIR) bins CC=arm-linux-gnueabi-gcc MOREFLAGS="-Werror"

examples:
	$(MAKE) -C $(LZ5DIR)
	$(MAKE) -C $(PRGDIR) lz5
	$(MAKE) -C examples test

endif
