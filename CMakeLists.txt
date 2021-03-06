# This is a cmake script. Process it with the CMake gui or command line utility
# to produce makefiles / Visual Studio project files on Mac OS X and Windows.
#
# To configure the build options either use the CMake gui, or run the command
# line utility including the "-i" option.

set(CMAKE_LEGACY_CYGWIN_WIN32 0)

cmake_minimum_required(VERSION 2.8)
# Only for version 3 and up. cmake_policy(SET CMP0042 NEW)

set (VERSION 1.5.5)

add_definitions (-DCMAKE -DVERSION=\"${VERSION}\")

if (WIN32)
	set (BINDIR .)
	set (SBINDIR .)
	set (SYSCONFDIR .)
	set (LIBDIR .)
	set (INCLUDEDIR include)
	set (DATAROOTDIR share)
	set (MANDIR man)
	set (SHAREDEST .)
	set (PKGCONFIGDIR "${LIBDIR}/pkgconfig")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
	add_definitions("-D_CRT_NONSTDC_NO_DEPRECATE")
else (WIN32)
	set (BINDIR bin)
	set (SBINDIR sbin)
	if ("${CMAKE_INSTALL_PREFIX}" STREQUAL /usr)
		set (SYSCONFDIR /etc/mosquitto)
	else ("${CMAKE_INSTALL_PREFIX}" STREQUAL /usr)
		set (SYSCONFDIR etc/mosquitto)
	endif ("${CMAKE_INSTALL_PREFIX}" STREQUAL /usr)
	
	set (LIBDIR lib${LIB_SUFFIX})
	set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIBDIR}")
	set (INCLUDEDIR include)
	set (DATAROOTDIR share)
	set (MANDIR "${DATAROOTDIR}/man")
	set (SHAREDIR "${DATAROOTDIR}/mosquitto")
	set (PKGCONFIGDIR "${LIBDIR}/pkgconfig")
endif (WIN32)

option(TARGET_ANDROID "Build for ANDROID?" OFF)
option(TARGET_IOS "Build for IOS?" OFF)
if(${TARGET_ANDROID} STREQUAL ON)
	add_definitions("-DTARGET_ANDROID")
	message(STATUS "target system android")
	set(CMAKE_TOOLCHAIN_FILE android/android.toolchain.cmake)
elseif(${TARGET_IOS} STREQUAL ON)
	message(STATUS "target system ios")
	set(CMAKE_TOOLCHAIN_FILE ios/ios.toolchain.cmake)
endif(${TARGET_ANDROID} STREQUAL ON)

SET (CMAKE_C_FLAGS "-g ${CMAKE_C_FLAGS}")

project(mosquitto)

option(WITH_TLS
	"Include SSL/TLS support?" ON)
option(WITH_TLS_PSK
	"Include TLS-PSK support (requires WITH_TLS)?" ON)
option(WITH_EC
	"Include Elliptic Curve support (requires WITH_TLS)?" ON)
if (${WITH_TLS} STREQUAL ON)
	if (${TARGET_ANDROID} STREQUAL ON)
		set(OPENSSL_INCLUDE_DIR ${mosquitto_SOURCE_DIR}/android/include)
		if ("${ANDROID_ABI}" STREQUAL arm)
			message(STATUS "ANDROID_ABI IS ARM")
			set(OPENSSL_LIBRARIES ${mosquitto_SOURCE_DIR}/android/lib/arm/libssl.a ${mosquitto_SOURCE_DIR}/android/lib/arm/libcrypto.a)
		elseif("${ANDROID_ABI}" STREQUAL arm64)
			message(STATUS "ANDROID_ABI IS ARM64")
			set(OPENSSL_LIBRARIES ${mosquitto_SOURCE_DIR}/android/lib/arm64/libssl.a ${mosquitto_SOURCE_DIR}/android/lib/arm64/libcrypto.a)
		endif("${ANDROID_ABI}" STREQUAL arm)
	elseif(${TARGET_IOS} STREQUAL ON)
		set(OPENSSL_INCLUDE_DIR ${mosquitto_SOURCE_DIR}/ios/include)
		set(OPENSSL_LIBRARIES ${mosquitto_SOURCE_DIR}/ios/lib/libssl.a ${mosquitto_SOURCE_DIR}/ios/lib/libcrypto.a)
	else()
		find_package(OpenSSL REQUIRED)
	endif(${TARGET_ANDROID} STREQUAL ON)
	add_definitions("-DWITH_TLS")

	if (${WITH_TLS_PSK} STREQUAL ON)
		add_definitions("-DWITH_TLS_PSK")
	endif (${WITH_TLS_PSK} STREQUAL ON)

	if (${WITH_EC} STREQUAL ON)
		add_definitions("-DWITH_EC")
	endif (${WITH_EC} STREQUAL ON)
else (${WITH_TLS} STREQUAL ON)
	set (OPENSSL_INCLUDE_DIR "")
endif (${WITH_TLS} STREQUAL ON)

option(WITH_SOCKS "Include SOCKS5 support?" ON)
if (${WITH_SOCKS} STREQUAL ON)
	add_definitions("-DWITH_SOCKS")
endif (${WITH_SOCKS} STREQUAL ON)

option(WITH_SRV "Include SRV lookup support?" OFF)

option(WITH_THREADING "Include client library threading support?" ON)
if (${WITH_THREADING} STREQUAL ON)
	add_definitions("-DWITH_THREADING")
	if (WIN32)
		if (CMAKE_CL_64)
			set (PTHREAD_LIBRARIES C:\\pthreads\\Pre-built.2\\lib\\x64\\pthreadVC2.lib)
		else (CMAKE_CL_64)
			set (PTHREAD_LIBRARIES C:\\pthreads\\Pre-built.2\\lib\\x86\\pthreadVC2.lib)
		endif (CMAKE_CL_64)
		set (PTHREAD_INCLUDE_DIR C:\\pthreads\\Pre-built.2\\include)
	elseif(${TARGET_ANDROID} STREQUAL ON)
		#set (PTHREAD_LIBRARIES "")
	else (WIN32)
		find_library(LIBPTHREAD pthread)
		if (LIBPTHREAD)
			set (PTHREAD_LIBRARIES pthread)
		else (LIBPTHREAD)
			set (PTHREAD_LIBRARIES "")
		endif()
		set (PTHREAD_INCLUDE_DIR "")
	endif (WIN32)
else (${WITH_THREADING} STREQUAL ON)
	set (PTHREAD_LIBRARIES "")
	set (PTHREAD_INCLUDE_DIR "")
endif (${WITH_THREADING} STREQUAL ON)

option(DOCUMENTATION "Build documentation?" ON)

# ========================================
# Include projects
# ========================================
add_subdirectory(lib)
add_subdirectory(msgpack)
add_subdirectory(clientcore)
if(${TARGET_ANDROID} STREQUAL ON)
	add_subdirectory(android)
elseif(${TARGET_IOS} STREQUAL ON)
	add_subdirectory(ios)
elseif(APPLE)
	add_subdirectory(src)
	add_subdirectory(client)
	add_subdirectory(pushlib)
else()
	add_definitions("-DWITH_EPOLL")
	add_subdirectory(src)
	add_subdirectory(client)
	add_subdirectory(pushlib)
endif(${TARGET_ANDROID} STREQUAL ON)
if (${DOCUMENTATION} STREQUAL ON)
	add_subdirectory(man)
endif (${DOCUMENTATION} STREQUAL ON)

# ========================================
# Install config file
# ========================================

install(FILES mosquitto.conf aclfile.example pskfile.example pwfile.example DESTINATION "${SYSCONFDIR}")


# ========================================
# Install pkg-config files
# ========================================

configure_file(libmosquitto.pc.in libmosquitto.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libmosquitto.pc" DESTINATION "${PKGCONFIGDIR}")
configure_file(libmosquittopp.pc.in libmosquittopp.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libmosquittopp.pc" DESTINATION "${PKGCONFIGDIR}")

# ========================================
# Testing
# ========================================
add_custom_target(Tests COMMAND make -C ${mosquitto_SOURCE_DIR}/test test)
