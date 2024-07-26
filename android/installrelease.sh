#!/bin/bash -e
# see also https://github.com/minetest/minetest/blob/8ff0e1da1578d4dea5b89227eca4ade39943c871/doc/developing/android.md
ANDROID_HOME=~/Android/Sdk ./gradlew assemblerelease
~/Android/Sdk/build-tools/33.0.2/apksigner sign --ks ~/.android/debug.keystore --ks-pass pass:android ./app/build/outputs/apk/release/app-arm64-v8a-release-unsigned.apk
~/Android/Sdk/platform-tools/adb install -r -d ./app/build/outputs/apk/release/app-arm64-v8a-release-unsigned.apk
