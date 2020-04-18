# p7zip-zstd

## **增加压缩算法 zstd 到 p7zip 中** ##

+ 实现7z的命令行版本（7za）支持 zstd 压缩算法。
   1. 实现 zstd 算法的融入，zstd 源码融入，将 zstd Register 进 p7zip 的压缩算法数组中。
   2. 实现 zstd 算法 Handler 接口层函数，并将 zstd Handler Register 进 p7zip Handler 数组中。

+ 实现包含 zstd 算法的 7z 库。

# 使用方法

## 7za 编译 使用 ##

+ 7za : Standalone version of 7-Zip console that supports only 7z/xz/cab/zip/gzip/bzip2/tar.
   1. 编译 7za ：工程根目录 make 7za 命令，将在根目录生成 bin 目录。
   2. 使用 7za 打包 ：./7za a test.7z TESTFILEorFOLDER (默认使用 zstd 压缩)
   3. 使用 7za 解包 ：./7za x test.7z