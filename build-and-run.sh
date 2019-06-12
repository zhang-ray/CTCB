#!/bin/bash


rm -rf ../CTCB-android-build
exit 1 | \
mkdir ../CTCB-android-build && cd ../CTCB-android-build

exit 1 | \
~/Android/Sdk/cmake/3.10.2.4988404/bin/cmake -DCMAKE_TOOLCHAIN_FILE=~/Android/Sdk/ndk-bundle/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=TRUE -DCMAKE_BUILD_TYPE=Release ../CTCB

# build 
exit 1 | \
make -j8

# push and run
adb push ctcb-test00 /system/bin/ \
&& \
adb shell ctcb-test00