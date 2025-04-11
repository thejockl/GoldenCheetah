#!/bin/bash
set -ev

cd src
echo "Checking GoldenCheetah.app can execute"
GoldenCheetah.app/Contents/MacOS/GoldenCheetah --version 2> GCversionMacOS.txt

echo "About to create dmg file and fix up"
mkdir GoldenCheetah.app/Contents/Frameworks

export DYLD_FALLBACK_LIBRARY_PATH=/opt/homebrew/lib:$DYLD_FALLBACK_LIBRARY_PATH

# Initial deployment using macdeployqt
macdeployqt GoldenCheetah.app -verbose=2 -fs=hfs+ -dmg -executable=GoldenCheetah.app/Contents/MacOS/GoldenCheetah

echo "Renaming dmg file to branch and build number ready for deploy"
# export FINAL_NAME=GoldenCheetah_v3.7_x64-gha.dmg
# mv GoldenCheetah.dmg $FINAL_NAME

# Add last commit message and SHA256 to txt file
git log -1 >> GCversionMacOS.txt
echo "SHA256 hash of $FINAL_NAME:" >> GCversionMacOS.txt
shasum -a 256 GoldenCheetah.dmg | cut -f 1 -d ' ' >> GCversionMacOS.txt
cat GCversionMacOS.txt

# ls -l $FINAL_NAME

exit
