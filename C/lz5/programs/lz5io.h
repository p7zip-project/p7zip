/*
  LZ5io.h - LZ5 File/Stream Interface
  Copyright (C) Yann Collet 2011-2015
  GPL v2 License

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

  You can contact the author at :
  - LZ5 source repository : https://github.com/inikep/lz5
  - LZ5 public forum : https://groups.google.com/forum/#!forum/lz5c
*/
/*
  Note : this is stand-alone program.
  It is not part of LZ5 compression library, it is a user code of the LZ5 library.
  - The license of LZ5 library is BSD.
  - The license of xxHash library is BSD.
  - The license of this source file is GPLv2.
*/

#pragma once

/* ************************************************** */
/* Special input/output values                        */
/* ************************************************** */
#define NULL_OUTPUT "null"
static char const stdinmark[] = "stdin";
static char const stdoutmark[] = "stdout";
#ifdef _WIN32
static char const nulmark[] = "nul";
#else
static char const nulmark[] = "/dev/null";
#endif


/* ************************************************** */
/* ****************** Functions ********************* */
/* ************************************************** */

int LZ5IO_compressFilename  (const char* input_filename, const char* output_filename, int compressionlevel);
int LZ5IO_decompressFilename(const char* input_filename, const char* output_filename);


int LZ5IO_compressMultipleFilenames(const char** inFileNamesTable, int ifntSize, const char* suffix, int compressionlevel);
int LZ5IO_decompressMultipleFilenames(const char** inFileNamesTable, int ifntSize, const char* suffix);

/* ************************************************** */
/* ****************** Parameters ******************** */
/* ************************************************** */

/* Default setting : overwrite = 1;
   return : overwrite mode (0/1) */
int LZ5IO_setOverwrite(int yes);

/* blockSizeID : valid values : 1-7
   return : -1 if error, blockSize if OK */
int LZ5IO_setBlockSizeID(int blockSizeID);

/* Default setting : independent blocks */
typedef enum { LZ5IO_blockLinked=0, LZ5IO_blockIndependent} LZ5IO_blockMode_t;
int LZ5IO_setBlockMode(LZ5IO_blockMode_t blockMode);

/* Default setting : no block checksum */
int LZ5IO_setBlockChecksumMode(int xxhash);

/* Default setting : stream checksum enabled */
int LZ5IO_setStreamChecksumMode(int xxhash);

/* Default setting : 0 (no notification) */
int LZ5IO_setNotificationLevel(int level);

/* Default setting : 0 (disabled) */
int LZ5IO_setSparseFile(int enable);

/* Default setting : 0 (disabled) */
int LZ5IO_setContentSize(int enable);
