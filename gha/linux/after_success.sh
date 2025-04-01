#!/bin/bash
set -ev


########################################################################
# This script should be run from GoldenCheetah src directory after build
cd src
if [ ! -x ./GoldenCheetah ]; then
	echo "Build GoldenCheetah and execute from distribution src"
	exit 1
fi

echo "Checking GoldenCheetah.app can execute"
./GoldenCheetah --version


####################################
# Create AppDir and start populating

mkdir -p appdir

# Executable
cp GoldenCheetah appdir

# Desktop file
cat > appdir/GoldenCheetah.desktop <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=GoldenCheetah
Comment=Cycling Power Analysis Software.
Exec=GoldenCheetah
Icon=gc
Categories=Science;Sports;
EOF

# Icon
cp Resources/images/gc.png appdir/


###########################################
# Download current version of linuxdeployqt

wget --no-verbose -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod a+x linuxdeployqt-continuous-x86_64.AppImage


##################
# Deploy to appdir

./linuxdeployqt-continuous-x86_64.AppImage appdir/GoldenCheetah \
	-qmake=/usr/bin/qmake6 \
	-verbose=2 \
	-bundle-non-qt-libs \
	-exclude-libs=libqsqlmysql,libqsqlpsql,libnss3,libnssutil3,libxcb-dri3.so.0 \
	-unsupported-allow-new-glibc

# Generate AppImage
wget --no-verbose "https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage"

# Fix RPATH on QtWebEngineProcess
patchelf --set-rpath '$ORIGIN/../lib' appdir/libexec/QtWebEngineProcess
chmod a+x appimagetool-x86_64.AppImage
./appimagetool-x86_64.AppImage appdir


#########
# Cleanup

rm linuxdeployqt-continuous-x86_64.AppImage
rm appimagetool-x86_64.AppImage
rm -rf appdir

if [ ! -x ./GoldenCheetah*.AppImage ] then
	echo "AppImage not generated, check the errors"
	exit 1
fi

echo "Renaming AppImage file to branch and build number ready for deploy"
export FINAL_NAME=GoldenCheetah_v3.7_x64.AppImage
mv GoldenCheetah*.AppImage $FINAL_NAME
ls -l $FINAL_NAME


##############
# Minimum Test

./$FINAL_NAME --version 2>GCversionLinux.txt
git log -1 >> GCversionLinux.txt
echo "SHA256 hash of $FINAL_NAME:" >> GCversionLinux.txt
shasum -a 256 $FINAL_NAME | cut -f 1 -d ' '  >> GCversionLinux.txt
cat GCversionLinux.txt
