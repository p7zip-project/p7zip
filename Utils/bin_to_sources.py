#!/usr/bin/env python

import fnmatch
import os
import sys

dir0='CPP/7zip/Bundles/Alone'
file0='Utils/file_7za.py'

dir0='CPP/7zip/Bundles/SFXCon'
file0='Utils/file_7zCon_sfx.py'

dir0='CPP/7zip/UI/Console'
file0='Utils/file_7z.py'

dir0='CPP/7zip/Compress/Rar'
file0='Utils/file_Codecs_Rar_so.py'

dir0='CPP/7zip/Bundles/Format7zFree'
file0='Utils/file_7z_so.py'

dir0='CPP/7zip/UI/GUI'
file0='Utils/file_7zG.py'

dir0='CPP/7zip/Bundles/Alone7z'
file0='Utils/file_7zr.py'

dir0='CPP/7zip/UI/FileManager'
file0='Utils/file_7zFM.py'

dir0='CPP/7zip/Bundles/LzmaCon'
file0='Utils/file_LzmaCon.py'

dir0='CPP/7zip/UI/Client7z'
file0='Utils/file_Client7z.py'

dir0='CPP/7zip/UI/P7ZIP'
file0='Utils/file_P7ZIP.py'

dir0='CPP/7zip/TEST/TestUI'
file0='Utils/file_TestUI.py'


listO=[]

for file in os.listdir(dir0):
	if fnmatch.fnmatch(file, '*.o'):
		# print(file)
		file=os.path.splitext(file)[0]
		listO.append(file)


listO.sort()

listc=[]
listcpp=[]

for file in listO:
	# print("Searching " + file + " ...")
	f_c= None
	f_cpp = None
	file_c = file + '.c'
	file_cpp = file + '.cpp'
	for root, dirs, files in os.walk("."):
		for f in files:
			if f == file_c:
				f_c=os.path.join(root, f)
			if f == file_cpp:
				f_cpp=os.path.join(root, f)
	if f_c is None:
		if f_cpp is None:
			print("Cannot find {}".format(file))
			sys.exit(-1)
		else:
			listcpp.append(f_cpp[2:])
	else:
		if f_cpp is None:
			listc.append(f_c[2:])
		else:
			print("error {} => {} and {}".format(file,f_c,f_cpp))
			sys.exit(-1)

f=open(file0,'w')
f.write('\n')

listc.sort()
listcpp.sort()

f.write('files_c=[\n')
for file in listc:
	f.write(" '{}',\n".format(file))
f.write(']\n')
f.write('\n')
f.write('files_cpp=[\n')
for file in listcpp:
	f.write(" '{}',\n".format(file))
f.write(']\n')
f.write('\n')
f.close()


