#!/bin/bash -e

old_wd="$(pwd)"

# rm -r "/home/gregor/minetest-android-test/downgraded-deps/minetest_android_deps/build/Irrlicht-arm64-v8a"

cd "/home/gregor/minetest-android-test/downgraded-deps/minetest_android_deps"
ANDROID_NDK=/home/gregor/Android/Sdk/ndk/25.2.9519653 ./build.sh Irrlicht arm64-v8a

cd "/home/gregor/minetest-android-test/android"
ANDROID_HOME=/home/gregor/Android/Sdk ./gradlew assembleDebug

/home/gregor/Android/Sdk/platform-tools/adb install /home/gregor/minetest-android-test/android/app/build/outputs/apk/debug/app-arm64-v8a-debug.apk
sleep 0.5
/home/gregor/Android/Sdk/platform-tools/adb shell am start -n net.minetest.minetest/net.minetest.minetest.MainActivity

cd "$old_wd"
