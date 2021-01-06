#!/bin/bash

#压缩程序可执行文件路径
program=$1
if [ ! $program ]
then
	program=../bin/7z
elif [[ $program = "-h" || $program = "--help" ]]
then
	echo "使用例子:     ./BenchMark.sh   ../bin/7z     silesia"
	echo "需要两个参数：                 压缩程序路径  被压缩的文件（夹）路径"
	exit 1
fi

#测试文件路径
sourcefile=$2
if [ ! $sourcefile ]
then
	sourcefile=silesia
fi

#获取文件或目录（中所有文件）的大小
FILESIZE=0
function getsize()
{
	if [ -d $1 ]
	then
		for file in `ls $1`
		do
			getsize $1"/"$file
		done
	elif [ -f $1 ]
	then
		total=`wc -c $1`
		index=`expr index "$total" '  '`
		len=`expr $index \- 1`
		size=${total:0:$len}
		FILESIZE=`expr $size + $FILESIZE`
	fi
}

#计算时间差，返回毫秒数
function difftime()
{
	s1=$(echo $1 | cut -d '.' -f 1)
	s2=$(echo $1 | cut -d '.' -f 2)
	e1=$(echo $2 | cut -d '.' -f 1)
	e2=$(echo $2 | cut -d '.' -f 2)
	ds=`expr $e1 \- $s1`
	dn=`expr $e2 \- $s2`
	ns=$(($ds * 1000000000 + dn))
	ms=$(($ns / 1000000))
	echo $ms
}

function usages()
{
	echo "使用示例:     ./BenchMark.sh   ../bin/7z     silesia"
	echo "需要两个参数：                 压缩程序路径  被压缩的文件（夹）路径"
	exit 1
}

#获取源文件或目录大小
FILESIZE=0
getsize $sourcefile
originfilesize=$FILESIZE

if [ ! -f $program ]
then
	echo $program is not exist
fi
if [[ ! -f $sourcefile && ! -d $sourcefile ]]
then
	echo $sourcefile \(file or folder\) is not exist
elif [ $originfilesize = '0' ]
then
	echo $outputfilename size is 0
fi
if [[ ! -f $program  || $originfilesize = '0' ]]
then
	usages
fi

#解压文件夹，分隔符
uncompressfoldername='uncompress'
tab=','

#输出文件
outputfilename=output.csv

#删除旧的输出文件，新建新的输出文件
if [ -f $outputfilename ]
then
	rm $outputfilename -rf
	if [ $? -ne 0 ]
	then 
		exit 1
	fi
fi
touch $outputfilename

#写入文件头
echo "format,method,encode_size(BYTE),encode_time(ms),encode_speed(M/s),decode_time(ms),decoded_speed(M/s),compression_ratio" >> $outputfilename

for format in zip 7z
do
	if [ $format = 7z ]
	then
		method="zstd flzma2 lzma lzma2 brotli lz4 lz5 lizard"
	else
		method="deflate lzma zstd"
	fi

	for codec in $method
	do
		#设置压缩等级
		maxlevel=9
		if [ $codec = 'zstd' ]
		then
			maxlevel=22
		elif [ $codec = 'flzma2' ]
		then
			maxlevel=9
		elif [ $codec = 'lzma' ]
		then
			maxlevel=9
		elif [ $codec = 'lzma2' ]
		then
			maxlevel=9
		elif [ $codec = 'brotli' ]
		then
			maxlevel=11
		elif [ $codec = 'lz4' ]
		then
			maxlevel=12
		elif [ $codec = 'lz5' ]
		then
			maxlevel=15
		elif [ $codec = 'lizard' ]
		then
			maxlevel=49
		elif [ $codec = 'deflate' ]
		then
			maxlevel=9
		fi
		
		#压缩输出文件名
		destfilename=$(basename $sourcefile)_${codec}_${maxlevel}.${format}
		
		#压缩并计算压缩时间、压缩速度
		starttime=`date +"%s.%N"`
		if [ $format = 7z ]
		then
			$program a -t$format $destfilename $sourcefile -m0=$codec -mx=$maxlevel
		else
			$program a -t$format $destfilename $sourcefile -mm=$codec -mx=$maxlevel
		fi
		endtime=`date +"%s.%N"`
		encodems=$(difftime $starttime $endtime)
		encodesec=`awk 'BEGIN{printf "%0.3f","'$encodems'"/1000}'`
		encodespeed=`awk 'BEGIN{printf "%0.2f","'$originfilesize'"/"'$encodesec'"/1024/1024}'`
		
		#获取压缩后文件大小
		FILESIZE=0
		getsize $destfilename
		encodefilesize=$FILESIZE
		
		#解压并计算解压时间、解压速度
		starttime=`date +"%s.%N"`
		$program x $destfilename -o$uncompressfoldername
		endtime=`date +"%s.%N"`
		decodems=$(difftime $starttime $endtime)
		decodesec=`awk 'BEGIN{printf "%0.3f","'$decodems'"/1000}'`
		decodespeed=`awk 'BEGIN{printf "%0.2f", "'$originfilesize'"/"'$decodesec'"/1024/1024}'`
		
		#计算压缩率
		compressratio=`awk 'BEGIN{printf "%0.2f","'$originfilesize'"/"'$encodefilesize'"}'`
		
		#输出结果到文件
		echo $format$tab$codec-$maxlevel$tab$encodefilesize$tab$encodems$tab$encodespeed$tab$decodems$tab$decodespeed$tab$compressratio >> $outputfilename
		diff $sourcefile ${uncompressfoldername}/$(basename $sourcefile) -r
		if [ $? -ne 0 ]
		then
			echo diff file $destfilename failed!!!
			exit 1
		fi
		
		rm -rf $uncompressfoldername
		rm -rf $destfilename
	done
done
































