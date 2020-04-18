# ! /bin/sh

# remove the strange filenames ...
rm -f -- "--" "bar" "-c" "-d" "-foo" "foo bar" "foo (bar) - ABCD" "-h" "-help"
if [ "$?" != "0" ] ; then
  echo "error during rm"
  exit 1
fi

# pipe
out=`echo "foobar" | ../p7zip |  ../p7zip -d`

if [ "${out}" != "foobar" ] ; then
  echo "error in ../p7zip |  ../p7zip -d"
  exit 1
fi

# path with "./"
echo "foobar content" > foobar
../p7zip ./foobar
if [ "$?" != "0" ] ; then
  echo "error p7zip ./foobar"
  exit 1
fi

if [ ! -f ./foobar.7z ] ; then
  echo "error ./foobar.7z not found"
  exit 1
fi

../p7zip -d ./foobar.7z
if [ "$?" != "0" ] ; then
  echo "error p7zip -d ./foobar.7z"
  exit 1
fi

if [ ! -f ./foobar ] ; then
  echo "error ./foobar not found"
  exit 1
fi

content=`cat foobar`
if [ "${content}" != "foobar content" ] ; then
  echo "error content of foobar"
  exit 1
fi

rm -f foobar

# the strange filenames are in the tar ...
tar xf files.tar
if [ "$?" != "0" ] ; then
  echo "error in tar xf files.tar"
  exit 1
fi

../p7zip -- "--" "bar" "-c" "-d" "-foo" "foo bar" "foo (bar) - ABCD" "-h" "-help"
if [ "$?" != "0" ] ; then
  echo "error in p7zip"
  exit 1
fi

for f in "--" "bar" "-c" "-d" "-foo" "foo bar" "foo (bar) - ABCD" "-h" "-help"
do
  7za -- t "${f}.7z" > /dev/null
  if [ "$?" != 0 ] ; then
    echo error in "${f}.7z"
    exit 1
  fi
done 

../p7zip -d -- *.7z
if [ "$?" != "0" ] ; then
  echo "error in p7zip -d"
  exit 1
fi

for f in "--" "bar" "-c" "-d" "-foo" "foo bar" "foo (bar) - ABCD" "-h" "-help"
do
  if [ ! -f "${f}" ] ; then
    echo error "${f}" not found
    exit 1
  fi
done

# remove the strange filenames ...
rm -f -- "--" "bar" "-c" "-d" "-foo" "foo bar" "foo (bar) - ABCD" "-h" "-help"
if [ "$?" != "0" ] ; then
  echo "error during rm"
  exit 1
fi

# test errors

../p7zip no_file
if [ "$?" = "0" ] ; then
  echo "error p7zip no_file"
  exit 1
fi

../p7zip -d no_file
if [ "$?" = "0" ] ; then
  echo "error p7zip -d no_file"
  exit 1
fi

../p7zip -d no_file.7z
if [ "$?" = "0" ] ; then
  echo "error p7zip -d no_file.7z"
  exit 1
fi

echo 
echo "########"
echo "All Done"
echo "########"

