# ##########################################################################
# Lizard programs - Makefile
# Copyright (C) Yann Collet 2011-2016
# Copyright (C) Przemyslaw Skibinski 2016-2017
#
# This Makefile is validated for Linux, macOS, *BSD, Hurd, Solaris, MSYS2 targets
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
# lizard : Command Line Utility, supporting gzip-like arguments
# lizardc32: Same as lizard, but forced to compile in 32-bits mode
# ##########################################################################

VOID = /dev/null
LIBDIR  := ../lib
ENTROPY_FILES := $(LIBDIR)/entropy/*.c
#TEST_CODEWORDS := $(LIBDIR)/test/*.h

CFLAGS  ?= -O3   # allows custom optimization flags. For example : CFLAGS="-O2 -g" make
CFLAGS  += -Wall -Wextra -Wundef -Wcast-qual -Wcast-align -Wshadow \
           -Wswitch-enum -Wdeclaration-after-statement -Wstrict-prototypes \
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



ifeq ($(shell $(CC) -v 2>&1 | grep -c "gcc version "), 1)
#ALIGN_LOOP = -falign-loops=32
endif


default: lizard

m32: lizardc32

native: lizard

all: native m32

# dependencies
$(LIBDIR)/lizard_compress.o: $(LIBDIR)/lizard_compress.c $(LIBDIR)/lizard_common.h $(LIBDIR)/lizard_compress*.h $(LIBDIR)/lizard_parser_*.h $(TEST_CODEWORDS)
	$(CC) $(FLAGS) $< -c -o $@

$(LIBDIR)/lizard_compress32.o: $(LIBDIR)/lizard_compress.c $(LIBDIR)/lizard_common.h $(LIBDIR)/lizard_compress*.h $(LIBDIR)/lizard_parser_*.h $(TEST_CODEWORDS)
	$(CC) -m32 $(FLAGS) $< -c -o $@

$(LIBDIR)/lizard_decompress.o: $(LIBDIR)/lizard_decompress.c $(LIBDIR)/lizard_compress.h $(LIBDIR)/lizard_common.h $(LIBDIR)/lizard_decompress*.h $(TEST_CODEWORDS)
	$(CC) $(FLAGS) $(ALIGN_LOOP) $< -c -o $@

$(LIBDIR)/lizard_decompress32.o: $(LIBDIR)/lizard_decompress.c $(LIBDIR)/lizard_compress.h $(LIBDIR)/lizard_common.h $(LIBDIR)/lizard_decompress*.h $(TEST_CODEWORDS)
	$(CC) -m32 $(FLAGS) $< -c -o $@



lizard: $(LIBDIR)/lizard_decompress.o $(LIBDIR)/lizard_compress.o $(LIBDIR)/lizard_frame.o $(LIBDIR)/xxhash/xxhash.o bench.o lizardio.o lizardcli.o datagen.o $(ENTROPY_FILES)
	$(CC) $(FLAGS) $^ -o $@$(EXT)

lizardnohuf: $(LIBDIR)/lizard_decompress.c $(LIBDIR)/lizard_compress.c $(LIBDIR)/lizard_frame.o $(LIBDIR)/xxhash/xxhash.o bench.c lizardio.o lizardcli.c datagen.o
	$(CC) -DLIZARD_NO_HUFFMAN $(FLAGS) $^ -o $@$(EXT)

lizardc32: $(LIBDIR)/lizard_decompress32.o $(LIBDIR)/lizard_compress32.o $(LIBDIR)/lizard_frame.c $(LIBDIR)/xxhash/xxhash.c bench.c lizardio.c lizardcli.c datagen.c $(ENTROPY_FILES)
	$(CC) -m32 $(FLAGS) $^ -o $@$(EXT)


clean:
	@$(MAKE) -C $(LIBDIR) $@ > $(VOID)
	@rm -f core *.o *.test tmp* \
        lizard$(EXT) lizardc32$(EXT) \
	@echo Cleaning completed


#-----------------------------------------------------------------------------
# make install is validated only for Linux, OSX, BSD, Hurd, Haiku and Solaris targets
#-----------------------------------------------------------------------------
ifneq (,$(filter $(shell uname),Linux Darwin GNU/kFreeBSD GNU OpenBSD FreeBSD NetBSD DragonFly SunOS Haiku))

unlizard: lizard
	ln -s lizard unlizard

lizardcat: lizard
	ln -s lizard lizardcat


ifneq (,$(filter $(shell uname),SunOS))
INSTALL ?= ginstall
else
INSTALL ?= install
endif

PREFIX  ?= /usr/local
DESTDIR ?=
BINDIR  ?= $(PREFIX)/bin

ifneq (,$(filter $(shell uname),OpenBSD FreeBSD NetBSD DragonFly SunOS))
MANDIR  ?= $(PREFIX)/man/man1
else
MANDIR  ?= $(PREFIX)/share/man/man1
endif

INSTALL_PROGRAM ?= $(INSTALL) -m 755
INSTALL_SCRIPT  ?= $(INSTALL) -m 755
INSTALL_MAN     ?= $(INSTALL) -m 644


install: lizard$(EXT)
	@echo Installing binaries
	@$(INSTALL) -d -m 755 $(DESTDIR)$(BINDIR)/ $(DESTDIR)$(MANDIR)/
	@$(INSTALL_PROGRAM) lizard$(EXT) $(DESTDIR)$(BINDIR)/lizard$(EXT)
	@ln -sf lizard$(EXT) $(DESTDIR)$(BINDIR)/lizardcat$(EXT)
	@ln -sf lizard$(EXT) $(DESTDIR)$(BINDIR)/unlizard$(EXT)
	@echo Installing man pages
	@$(INSTALL_MAN) -m 644 lizard.1 $(DESTDIR)$(MANDIR)/lizard.1
	@ln -sf lizard.1 $(DESTDIR)$(MANDIR)/lizardcat.1
	@ln -sf lizard.1 $(DESTDIR)$(MANDIR)/unlizard.1
	@echo lizard installation completed

uninstall:
	@$(RM) $(DESTDIR)$(BINDIR)/lizardcat$(EXT)
	@$(RM) $(DESTDIR)$(BINDIR)/unlizard$(EXT)
	@$(RM) $(DESTDIR)$(BINDIR)/lizard$(EXT)
	@$(RM) $(DESTDIR)$(MANDIR)/lizard.1
	@$(RM) $(DESTDIR)$(MANDIR)/lizardcat.1
	@$(RM) $(DESTDIR)$(MANDIR)/unlizard.1
	@echo lizard programs successfully uninstalled

endif
