#!/bin/sh

rpmbuild -bb p7zip.spec --define "_makedir $PWD/../" --define "_tempplace $PWD"

# lists are written here and for debugging one can uncomment the following line
rm -f *_list
