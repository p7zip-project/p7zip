Name:           p7zip
Version:        17.04
Release:        0
Summary:        7-zip file compression program
License:        LGPL-2.1-or-later
Group:          Productivity/Archiving/Compression
URL:            https://github.com/jinfeihan57/p7zip

%define PREFIX /usr/local
%define templist %{_tempplace}/tmp_list
%define finallist %{_tempplace}/entire_list

%description
p7zip is a quick port of 7z.exe and 7za.exe (command line version of
7zip, see www.7-zip.org) for Unix. 7-Zip is a file archiver with
highest compression ratio. Since 4.10, p7zip (like 7-zip) supports
little-endian and big-endian machines.

%build
pushd  %{_makedir}

N_PROCS=`cat /proc/cpuinfo | grep processor | wc -l`
make -j $N_PROCS 7z 7zr 7za

popd

%install
pushd %{_makedir}

make DEST_DIR=%{buildroot} install

popd

pushd %{buildroot}

find . -type d -name p7zip > %{templist}
find . -type f >> %{templist}

# remove the starting dot
cat %{templist} | cut -d . -f 2- > %{finallist}

popd

%files -f %{finallist}

%changelog
