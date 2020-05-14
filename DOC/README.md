# **p7zip-Advanced**

## 增加压缩算法到 p7zip 中 

+ 实现7z的命令行版本（7za）支持多种压缩算法。
   1. 实现 zstd 算法的融入，zstd 源码融入，将 zstd Register 进 p7zip 的压缩算法数组中。
   2. 实现 zstd 算法 Archive 接口层函数，并将 zstd Archive Register 进 p7zip Archive 数组中。
   3. 实现 lz4 算法的融入，lz4 源码融入，将 lz4 Register 进 p7zip 的压缩算法数组中。
   4. 实现 lz4 算法 Archive 接口层函数，并将 lz4 Archive Register 进 p7zip Archive 数组中。

+ 实现多种算法的 7z.zo 库以及 7z 可执行程序
   1. 实现 zstd 算法的 7z.zo 库和对应 7z 可执行程序。
   2. 实现 lz4 算法的 7z.zo 库和对应 7z 可执行程序。

# **使用方法**

## p7zip-Advanced 编译 & 使用 

+ 7za : Standalone version of 7-Zip console that supports only 7z/xz/cab/zip/gzip/bzip2/tar.
   1. 编译 7za ：工程根目录 make 7za 命令，将在根目录生成 bin 目录。
   2. 使用 7za 打包 ：./7za a test.7z TESTFILEorFOLDER (默认使用 zstd 压缩)
   3. 使用 7za 解包 ：./7za x test.7z
+ 7z : 7-Zip console that supports 7z/APM/Ar/Arj/bzip2/Cab/Chm/Hxs/Compound/Cpio/CramFS/Dmg/ELF/Ext/FAT/FLV/gzip/GPT/HFS/IHex/Iso/Lzh/lzma/lzma86/MachO/MBR/MsLZ/Mub/Nsis/NTFS/PE/TE/Ppmd/QCOW/Rar/Rar5/Rpm/Split/SquashFS/SWFc/SWF/tar/Udf/UEFIc/UEFIf/VDI/VHD/VMDK/wim/Xar/xz/Z/zip 
   1. 编译 7z ：工程根目录 make 7z 命令，将在根目录生成 bin 目录。
   2. 使用 7z 打包 ：./7z a test.7z TESTFILEorFOLDER (默认使用 zstd 压缩)
   3. 使用 7z 解包 ：./7z x test.7z
+ 7z or 7za 指定压缩算法 -m0=xxx 其中xxx为算法名忽略大小写
   1. 指定 zstd 压缩算法
      ./7z a test.7z TESTFILEorFOLDER -m0=zstd
   2. 指定 lz4 压缩算法
      ./7z a test.7z TESTFILEorFOLDER -m0=lz4
   2. 指定 flzma2 压缩算法
      ./7z a test.7z TESTFILEorFOLDER -m0=flzma2

## FAQ 
+ 运行环境要求 
   1. 7z 7za 程序只能在类UNIX环境运行，如果采用cygwin编译则只能在cygwin终端运行。无法在windows终端或者powershell终端运行。

# **p7zip 规划**

## **p7zip 划分**

<table >
   <tr >
	    <th colspan="7">p7zip</th>
	</tr>
	<tr>
	    <th >描述</th>
	    <th colspan="4">CLI</th>
	    <th colspan="2">GUI</th>  
	</tr>
	<tr >
	    <th >名称</th>
	    <td  text-align:center>7za</td>
	    <td colspan="3">7z</td>
	    <td >7zG</td>
	    <td >7zFM</td>
	</tr>
	<tr>
	    <th rowspan="2">运行条件</th>
	    <td rowspan="2">7za 采用静态编译，不依赖任何动态库运行</td>
	    <td>7z</td>
	    <td>7z.so</td>
	    <td>Rar.so</td>
	    <td rowspan="5">暂无规划</td>
	    <td rowspan="5">7z文件管理</td>
	</tr>
	<tr>
	    <td colspan="3">7z 依赖 7z.ao & Rar.so 运行</td>
	</tr>
	<tr>
	    <th>支持格式</th>
	    <td >7z/xz/cab/zip/gzip/bzip2/tar</td>
	    <td style="table-layout:fixed" width="200" colspan="3">7z/APM/Ar/Arj/bzip2/Cab/Chm/Hxs/Compound/
       Cpio/CramFS/Dmg/ELF/Ext/FAT/FLV/gzip/GPT/HFS/
       IHex/Iso/Lzh/lzma/lzma86/MachO/MBR/MsLZ/Mub/
       Nsis/NTFS/PE/TE/Ppmd/QCOW/Rar/Rar5/Rpm/Split/
       SquashFS/SWFc/SWF/tar/Udf/UEFIc/UEFIf/VDI/VHD/
       VMDK/wim/Xar/xz/Z/zip </td>
	</tr>
	<tr><th>支持编解码算法</th>
	    <td >BZip2/BCJ2/BCJ/PPC/IA64/ARM/
       ARMT/SPARC/Swap2/Swap4/Copy/
       Deflate64/Deflate/Delta/LZMA2/
       LZMA/PPMD/7zAES/AES256CBC</td>
	    <td style="table-layout:fixed" width="200" colspan="3">BZip2/BCJ2/BCJ/PPC/IA64/ARM/ARMT/SPARC/
       Swap2/Swap4/Copy/Deflate64/Deflate/Delta/LZMA2/
       LZMA/PPMD/7zAES/AES256CBC/Rar1/Rar2/Rar3/Rar5</td>
	</tr>
	<tr>
	    <th>支持Hash算法</th>
	    <td>CRC32/SHA1/SHA256/CRC64</td>
	    <td colspan="3">BLAKE2sp/CRC32/SHA1/SHA256/CRC64</td>
	</tr>
</table>

## **CLI工作规划**

<table >
   <tr >
	     <th rowspan="20">CLI工作规划</th>
	     <th >维护方向</th>
	     <th >条目</th>
	     <th >条目说明</th>
	     <th >注意事项</th>
	     <th >完成情况</th>
	</tr>
	<tr>
	    <td rowspan="8">增加算法至 p7zip中</td>
	    <td >ZSTD</td>
	    <td rowspan="8">在7zip-zstd基础上择优加入p7zip</td>
	    <td style="table-layout:fixed" width="100" rowspan="13">所有维护工作
                        都必须考虑
                        7za & 7z两
                        个可执行程
                        序的编译运
                        行，对标7zip
                        版本更新时注
                        意GUI更新和
                        CLI更新,优
                        先做CLI</td>
	    <td >7za完成 7z完成</td>
	</tr>
	<tr >
	    <td >Brotli</td>
	    <td >计划中</td>
	</tr>
	<tr >
	    <td >LZ4</td>
	    <td >7za完成 7z完成</td>
	</tr>
	<tr >
	    <td >LZ5</td>
	    <td >计划中</td>
	</tr>
	<tr >
	    <td >lzham</td>
	    <td >计划中</td>
	</tr>
	<tr >
	    <td >snappy</td>
	    <td >计划中</td>
	</tr>
	<tr >
	    <td >Fast Lzma2</td>
	    <td >7za完成 7z完成</td>
	</tr>
	<tr >
	    <td >Lizard</td>
	    <td >计划中</td>
	</tr>
	<tr>
	    <td rowspan="5">根据 7zip 版本对 p7zip进行版本对齐维护</td>
	    <td style="table-layout:fixed" width="200" >17.01:新增NewHandler.h / NewHandler.cpp重新定义了new(),针对老版本的_MSC_VER < 1900,修改了C/7zTypes.h中变量名，一些造成崩溃的BUG修复</td>
	    <td rowspan="5">根据紧迫度和难度分步迭代优化</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td style="table-layout:fixed" width="200" >18.03：汇编优化的LZMA 解压缩函数，在数据块独立时，支持对LZMA2/xz的多线程打包</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td style="table-layout:fixed" width="200" >18.05：LZMA&LZMA2压缩速度提升，修复7zip在 LargePages模式的BUG</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td style="table-layout:fixed" width="200" >18.06：修复18.05中的xz的多线程内存泄漏，提升压缩速度</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td style="table-layout:fixed" width="200" >19.00：增强加密，初始随机值有64big升值128bit，&修复一些BUG</td>
	    <td>计划中</td>
	</tr>
	<tr>
	   <td rowspan="6">优化算法</td>
	    <td>CRC32</td>
	   <td rowspan="5">采用SIMD优化加密</td>
	   <td style="table-layout:fixed" width="100" rowspan="5">在加密算法中
                      需要注意7zip
                      中的加密方法
                      是否与simd中
                      的加密逻辑
                      一致</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td>CRC64</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td>AES</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td>sha1</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td>sha256</td>
	    <td>计划中</td>
	</tr>
	<tr>
	    <td>其他优化方案</td>
	    <td >-</td>
	    <td >-</td>
	    <td >-</td>
	</tr>
</table>

