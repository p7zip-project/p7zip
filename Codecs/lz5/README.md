Introduction
-------------------------

LZ5 is a modification of [LZ4] which gives a better ratio at cost of slower compression and decompression speed. 
**In my experiments there is no open-source bytewise compressor that gives better ratio than lz5hc.**
The improvement in compression ratio is caused mainly because of:
- 22-bit dictionary instead of 16-bit in LZ4
- using 4 new parsers (including an optimal parser) optimized for a bigger dictionary
- support for 3-byte long matches (MINMATCH = 3)
- a special 1-byte codeword for the last occured offset

|Branch      |Status   |
|------------|---------|
|master      | [![Build Status][travisMasterBadge]][travisLink] [![Build status][AppveyorMasterBadge]][AppveyorLink]  |
|dev         | [![Build Status][travisDevBadge]][travisLink]    [![Build status][AppveyorDevBadge]][AppveyorLink]     |

[travisMasterBadge]: https://travis-ci.org/inikep/lz5.svg?branch=master "Continuous Integration test suite"
[travisDevBadge]: https://travis-ci.org/inikep/lz5.svg?branch=dev "Continuous Integration test suite"
[travisLink]: https://travis-ci.org/inikep/lz5
[AppveyorMasterBadge]: https://ci.appveyor.com/api/projects/status/o0ib75nwokjiui36/branch/master?svg=true "Visual test suite"
[AppveyorDevBadge]: https://ci.appveyor.com/api/projects/status/o0ib75nwokjiui36/branch/dev?svg=true "Visual test suite"
[AppveyorLink]: https://ci.appveyor.com/project/inikep/lz5
[LZ4]: https://github.com/Cyan4973/lz4


Benchmarks
-------------------------

In our experiments decompression speed of LZ5 is from 600-1600 MB/s. It's slower than LZ4 but much faster than zstd and brotli.
With the compresion ratio is opposite: LZ5 is better than LZ4 but worse than zstd and brotli.

| Compressor name             | Compression| Decompress.| Compr. size | Ratio |
| ---------------             | -----------| -----------| ----------- | ----- |
| memcpy                      |  8533 MB/s |  8533 MB/s |   104857600 |100.00 |
| lz4 r131                    |   480 MB/s |  2275 MB/s |    64872315 | 61.87 |
| lz4hc r131 -1               |    82 MB/s |  1896 MB/s |    59448496 | 56.69 |
| lz4hc r131 -3               |    54 MB/s |  1932 MB/s |    56343753 | 53.73 |
| lz4hc r131 -5               |    41 MB/s |  1969 MB/s |    55271312 | 52.71 |
| lz4hc r131 -7               |    31 MB/s |  1969 MB/s |    54889301 | 52.35 |
| lz4hc r131 -9               |    24 MB/s |  1969 MB/s |    54773517 | 52.24 |
| lz4hc r131 -11              |    20 MB/s |  1969 MB/s |    54751363 | 52.21 |
| lz4hc r131 -13              |    17 MB/s |  1969 MB/s |    54744790 | 52.21 |
| lz4hc r131 -15              |    14 MB/s |  2007 MB/s |    54741827 | 52.21 |
| lz5 v1.4                    |   191 MB/s |   892 MB/s |    56183327 | 53.58 |
| lz5hc v1.4 level 1          |   468 MB/s |  1682 MB/s |    68770655 | 65.58 |
| lz5hc v1.4 level 2          |   337 MB/s |  1574 MB/s |    65201626 | 62.18 |
| lz5hc v1.4 level 3          |   232 MB/s |  1330 MB/s |    61423270 | 58.58 |
| lz5hc v1.4 level 4          |   129 MB/s |   894 MB/s |    55011906 | 52.46 |
| lz5hc v1.4 level 5          |    99 MB/s |   840 MB/s |    52790905 | 50.35 |
| lz5hc v1.4 level 6          |    41 MB/s |   894 MB/s |    52561673 | 50.13 |
| lz5hc v1.4 level 7          |    35 MB/s |   875 MB/s |    50947061 | 48.59 |
| lz5hc v1.4 level 8          |    23 MB/s |   812 MB/s |    50049555 | 47.73 |
| lz5hc v1.4 level 9          |    17 MB/s |   727 MB/s |    48718531 | 46.46 |
| lz5hc v1.4 level 10         |    13 MB/s |   728 MB/s |    48109030 | 45.88 |
| lz5hc v1.4 level 11         |  9.18 MB/s |   719 MB/s |    47438817 | 45.24 |
| lz5hc v1.4 level 12         |  7.96 MB/s |   752 MB/s |    47063261 | 44.88 |
| lz5hc v1.4 level 13         |  5.38 MB/s |   710 MB/s |    46383307 | 44.23 |
| lz5hc v1.4 level 14         |  4.12 MB/s |   669 MB/s |    45843096 | 43.72 |
| lz5hc v1.4 level 15         |  2.16 MB/s |   619 MB/s |    45767126 | 43.65 |
| zstd v0.5.0 level 1         |   249 MB/s |   569 MB/s |    51121791 | 48.75 |
| zstd v0.5.0 level 2         |   177 MB/s |   523 MB/s |    49692088 | 47.39 |
| zstd v0.5.0 level 5         |    72 MB/s |   491 MB/s |    46373509 | 44.23 |
| zstd v0.5.0 level 9         |    17 MB/s |   523 MB/s |    43876466 | 41.84 |
| zstd v0.5.0 level 13        |    10 MB/s |   524 MB/s |    42305338 | 40.35 |
| zstd v0.5.0 level 17        |  3.21 MB/s |   524 MB/s |    41990713 | 40.05 |
| zstd v0.5.0 level 20        |  2.76 MB/s |   495 MB/s |    41862877 | 39.92 |
| brotli 2015-10-29 -1        |    86 MB/s |   208 MB/s |    47882059 | 45.66 |
| brotli 2015-10-29 -3        |    60 MB/s |   214 MB/s |    47451223 | 45.25 |
| brotli 2015-10-29 -5        |    17 MB/s |   217 MB/s |    43363897 | 41.36 |
| brotli 2015-10-29 -7        |  4.80 MB/s |   227 MB/s |    41222719 | 39.31 |
| brotli 2015-10-29 -9        |  2.23 MB/s |   222 MB/s |    40839209 | 38.95 |

The above results are obtained with [lzbench] using 1 core of Intel Core i5-4300U, Windows 10 64-bit (MinGW-w64 compilation under gcc 4.8.3) with 3 iterations. 
The ["win81"] input file (100 MB) is a concatanation of carefully selected files from installed version of Windows 8.1 64-bit. 

[lzbench]: https://github.com/inikep/lzbench
["win81"]: https://docs.google.com/uc?id=0BwX7dtyRLxThRzBwT0xkUy1TMFE&export=download


Documentation
-------------------------

The raw LZ5 block compression format is detailed within [lz5_Block_format].

To compress an arbitrarily long file or data stream, multiple blocks are required.
Organizing these blocks and providing a common header format to handle their content
is the purpose of the Frame format, defined into [lz5_Frame_format].
Interoperable versions of LZ5 must respect this frame format.

[lz5_Block_format]: lz5_Block_format.md
[lz5_Frame_format]: lz5_Frame_format.md
