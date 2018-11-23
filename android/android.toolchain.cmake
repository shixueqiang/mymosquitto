# Android 5.0 以上需要在此处设置 PIE
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")

option(ANDROID_ABI "ANDROID ABI?" arm)
set(CMAKE_ANDROID_NDK /home/shixq/android/android-ndk-r14b)
set(CMAKE_SYSTEM_NAME Android)
if(${ANDROID_ABI} STREQUAL arm)
	set(CMAKE_SYSTEM_VERSION 19)
	set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
elseif(${ANDROID_ABI} STREQUAL arm64)
	set(CMAKE_SYSTEM_VERSION 21)
	set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
endif(${ANDROID_ABI} STREQUAL arm)
# 配置使用 NDK Standalone Toolchain 编译
# set(NDK_STANDALONE_TOOLCHAIN /home/shixq/android/arm-android-toolchain)
# set(CMAKE_C_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/arm-linux-androideabi-clang)
# set(CMAKE_CXX_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/arm-linux-androideabi-clang++)
# set(CMAKE_FIND_ROOT_PATH ${NDK_STANDALONE_TOOLCHAIN})
# # 使用 NDK 提供的头文件
# add_definitions("--sysroot=${NDK_STANDALONE_TOOLCHAIN}/sysroot")