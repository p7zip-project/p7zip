LZ5 Block Format Description
============================
Last revised: 2016-01-22
Authors : Yann Collet, Przemyslaw Skibinski


This specification is intended for developers
willing to produce LZ5-compatible compressed data blocks
using any programming language.

LZ5 is an LZ77-type compressor with a fixed, byte-oriented encoding.
There is no entropy encoder back-end nor framing layer.
The latter is assumed to be handled by other parts of the system (see [LZ5 Frame format]).
This design is assumed to favor simplicity and speed.
It helps later on for optimizations, compactness, and features.

This document describes only the block format,
not how the compressor nor decompressor actually work.
The correctness of the decompressor should not depend
on implementation details of the compressor, and vice versa.

[LZ5 Frame format]: lz5_Frame_format.md


Compressed block format
-----------------------
An LZ5 compressed block is composed of sequences.
A sequence is a suite of literals (not-compressed bytes),
followed by a match copy.

Each sequence starts with a token.
The token is a one byte value. It is separated into flag (1, 00, 010, 011), literal length (LL, LLL) and match length (MMM) fields.
LZ5 uses 4 types of codewords from 1 to 4+ bytes long:
- [1_OO_LL_MMM] [OOOOOOOO] - 10-bit offset, 3-bit match length, 2-bit literal length
- [00_LLL_MMM] [OOOOOOOO] [OOOOOOOO] - 16-bit offset, 3-bit match length, 3-bit literal length
- [010_LL_MMM] [OOOOOOOO] [OOOOOOOO] [OOOOOOOO] - 24-bit offset, 3-bit match length, 2-bit literal length
- [011_LL_MMM] - last offset, 3-bit match length, 2-bit literal length

LZ5 uses different output codewords and is not compatible with LZ4. LZ4 output codewords are 3 byte long (24-bit) and look as follows:
- LLLL_MMMM OOOOOOOO OOOOOOOO - 16-bit offset, 4-bit match length, 4-bit literal length 

The flag field (1, 00, 010, 011) selects the format of a codeword: the lenght of the literal length field and the length of offset.

The literal length field (LL, LLL) uses the 2 or 3 bits of the token.
It provides the length of literals to follow.
Therefore each field ranges from 0 to 3 (2-bit) or 0 to 7 (3-bit).
If the field value is 0, then there is no literal.
If it is 3 (2-bit) or 7 (3-bit), then we need to add some more bytes to indicate the full length.
Each additional byte then represent a value from 0 to 255,
which is added to the previous value to produce a total length.
When the byte value is 255, another byte is output.
There can be any number of bytes following the token. There is no "size limit".
(Side note : this is why a not-compressible input block is expanded by 0.4%).

Example 1 : A length of 48 for 3-bit literal length will be represented as :

  - 7  : value for the 3-bits LLL field
  - 41 : (=48-7) remaining length to reach 48

Example 2 : A length of 280 for 3-bit literal length will be represented as :

  - 7   : value for the 3-bits LLL field
  - 255 : following byte is maxed, since 280-7 >= 255
  - 18  : (=280 - 7 - 255) ) remaining length to reach 280

Example 3 : A length of 7 for 3-bit literal length will be represented as :

  - 7  : value for the 3-bits LLL field
  - 0  : (=7-7) yes, the zero must be output

Following the token and optional length bytes, are the literals themselves.
They are exactly as numerous as previously decoded (length of literals).
It's possible that there are zero literal.

Following the literals is the match copy operation.

It starts by the offset. The length of offset depends on the flag field.
The flag "011" informs that decoder should use the last encoded offset.
The flag "00" informs that the offset is a 2 bytes value (16-bit), in little endian format.
The flag "010" informs that the offset is a 3 bytes value (24-bit), in little endian format.
The flag "1" informs that the offset is a 1 bytes value.
With additional 2-bits from the token it creates 10-bit offset (2-bits are high bits).

The offset represents the position of the match to be copied from.
1 means "current position - 1 byte".
The maximum offset value is (1<<24)-1, 1<<24 cannot be coded.
Note that 0 is an invalid value, not used. 

Then we need to extract the match length.
For this, we use the match length field from the token, the low 3-bits.
Value, obviously, ranges from 0 to 7.
However here, 0 means that the copy operation will be minimal.
The minimum length of a match, called minmatch, is 3.
As a consequence, a 0 value means 3 bytes, and a value of 7 means 10+ bytes.
Similar to literal length, on reaching the highest possible value (7), 
we output additional bytes, one at a time, with values ranging from 0 to 255.
They are added to total to provide the final match length.
A 255 value means there is another byte to read and add.
There is no limit to the number of optional bytes that can be output this way.
(This points towards a maximum achievable compression ratio of about 250).

With the offset and the matchlength,
the decoder can now proceed to copy the data from the already decoded buffer.
On decoding the matchlength, we reach the end of the compressed sequence,
and therefore start another one.


Parsing restrictions
-----------------------
There are specific parsing rules to respect in order to remain compatible
with assumptions made by the decoder :

1. The last 5 bytes are always literals
2. The last match must start at least 12 bytes before end of block.   
   Consequently, a block with less than 13 bytes cannot be compressed.

These rules are in place to ensure that the decoder
will never read beyond the input buffer, nor write beyond the output buffer.

Note that the last sequence is also incomplete,
and stops right after literals.


Additional notes
-----------------------
There is no assumption nor limits to the way the compressor
searches and selects matches within the source data block.
It could be a fast scan, a multi-probe, a full search using BST,
standard hash chains or MMC, well whatever.

Advanced parsing strategies can also be implemented, such as lazy match,
or full optimal parsing.

All these trade-off offer distinctive speed/memory/compression advantages.
Whatever the method used by the compressor, its result will be decodable
by any LZ5 decoder if it follows the format specification described above.
