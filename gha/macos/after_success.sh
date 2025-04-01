#!/bin/bash
set -ev

cd src
echo "Checking GoldenCheetah.app can execute"
GoldenCheetah.app/Contents/MacOS/GoldenCheetah --version 2> GCversionMacOS.txt
cat GCversionMacOS.txt

echo "About to create dmg file and fix up"
mkdir GoldenCheetah.app/Contents/Frameworks

install_name_tool -add_rpath "$(brew --prefix qt)/lib" libqcocoa.dylib

# Initial deployment using macdeployqt
macdeployqt6 GoldenCheetah.app -verbose=2 -executable=GoldenCheetah.app/Contents/MacOS/GoldenCheetah

tree GoldenCheetah.app

echo "Renaming dmg file to branch and build number ready for deploy"
export FINAL_NAME=GoldenCheetah_v3.7_x64.dmg
mv GoldenCheetah.dmg $FINAL_NAME
ls -l $FINAL_NAME

# Add last commit message and SHA256 to txt file
git log -1 >> GCversionMacOS.txt
echo "SHA256 hash of $FINAL_NAME:" >> GCversionMacOS.txt
shasum -a 256 $FINAL_NAME | cut -f 1 -d ' ' >> GCversionMacOS.txt
cat GCversionMacOS.txt

exit
