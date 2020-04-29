
# README

This is the Github Page of [p7-Zip] with support of additional Codecs. The library used therefore is located here: [Multithreading Library](https://github.com/mcmilk/zstdmt)

# status
| [Ubuntu][ubuntu-link]|
|---------------|
| ![ubuntu-badge]  |

[ubuntu-link]: https://github.com/2273816832/test_github_action/actions?query=workflow%3AUbuntu "UbuntuAction"
[ubuntu-badge]: https://github.com/2273816832/test_github_action/workflows/Ubuntu/badge.svg "Ubuntu"

## Codec overview
1. [Zstandard] v1.4.4 is a real-time compression algorithm, providing high compression ratios. It offers a very wide range of compression / speed trade-off, while being backed by a very fast decoder.
   - Levels: 1..22

2. [LZ4] v1.9.2 is lossless compression algorithm, providing compression speed at 400 MB/s per core (0.16 Bytes/cycle). It features an extremely fast decoder, with speed in multiple GB/s per core (0.71 Bytes/cycle). A high compression derivative, called LZ4_HC, is available, trading customizable CPU time for compression ratio.
   - Levels: 1..12

### install CLI
#### (Currently only supports CLI, if you want to do GUI please contact us)
1. git clone https://github.com/szcnick/p7zip.git
2. cd p7zip && make 7z . (OR make 7za)
3. ./bin/7z i . (OR 7za i)

The output should look like this:
```
7-Zip (a) [64] 17.00 : Copyright (c) 1999-2016 Igor Pavlov 
p7zip Version 17.00 (locale=zh_CN.UTF-8,Utf16=on,HugeFiles=on,64 bits,12 CPUs x64)
 
Formats:
...
 0 CK            xz       xz txz (.tar) FD 7 z X Z 00
 0               Z        z taz (.tar)  1F 9D
 0 CK            zstd     zst tzstd (.tar) 0 x F D 2 F B 5 2 5 . . 0 x F D 2 F B 5 2 8 00
 0 C   F         7z       7z            7 z BC AF ' 1C
 0     F         Cab      cab           M S C F 00 00 00 00
...

Codecs:
 0 4ED  303011B BCJ2
 0  ED  3030103 BCJ
 0  ED  3030205 PPC
 0  ED  3030401 IA64
 0  ED  3030501 ARM
 0  ED  3030701 ARMT
 0  ED  3030805 SPARC
 0  ED    20302 Swap2
 0  ED    20304 Swap4
 0  ED    40202 BZip2
 0  ED        0 Copy
 0  ED    40109 Deflate64
 0  ED    40108 Deflate
 0  ED        3 Delta
 0  ED       21 LZMA2
 0  ED    30101 LZMA
 0  ED    30401 PPMD
 0   D    40301 Rar1
 0   D    40302 Rar2
 0   D    40303 Rar3
 0   D    40305 Rar5
 0  ED  4F71104 LZ4
 0  ED  4F71101 ZSTD
 0  ED  6F10701 7zAES
 0  ED  6F00181 AES256CBC
 1 3ED  4F712FF RawSplitter

Hashers:
 0    4        1 CRC32
 0   20      201 SHA1
 0   32        A SHA256
 0    8        4 CRC64
 0   32      202 BLAKE2sp
```

### Usage (codec plugin)

- compression and decompression for [LZ4] and [Zstandard] within the p7-Zip container format
- you can only create `.7z` files, the files like `.lz4` and `.zst` are not covered by the plugins
- when compressing binaries (*.exe, *.dll), you have to explicitly disable the bcj2 filter via `-m0=bcj`.
- so the usage should look like this:
```
7z a archiv.7z -m0=bcj -m1=zstd -mx1   Fast mode, with BCJ preprocessor on executables
7z a archiv.7z -m0=bcj -m1=zstd -mx..  ...
7z a archiv.7z -m0=bcj -m1=zstd -mx21  2nd Slowest Mode, with BCJ preprocessor on executables
7z a archiv.7z -m0=bcj -m1=zstd -mx22  Ultra Mode, with BCJ preprocessor on executables
7z a archiv.7z -m0=bcj -m1=lz4 -mxN  ...
```

## License and redistribution

- the same as the Mainline [p7-Zip], which means GNU LGPL

## Links

- [7-Zip Homepage](https://www.7-zip.org/)
- [Request for inclusion](https://sourceforge.net/p/sevenzip/discussion/45797/thread/a7e4f3f3/) into the mainline 7-Zip:
  - result, will currently not included :(

## Version Information

- 7-Zip ZS Version 17.00
  - [LZ4] Version 1.9.2
  - [Zstandard] Version 1.4.4

## Working Plan
 - [check here](https://github.com/szcnick/p7zip/tree/dev/DOC)

[p7-Zip]:https://www.7-zip.org/
[LZ4]:https://github.com/lz4/lz4/
[Zstandard]:https://github.com/facebook/zstd/
