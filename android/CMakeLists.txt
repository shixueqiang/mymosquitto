include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/lib ${mosquitto_SOURCE_DIR}/clientcore
			${STDBOOL_H_PATH} ${STDINT_H_PATH})
link_directories(${mosquitto_BINARY_DIR}/lib)

set(LOG_LIBRARIES ${CMAKE_ANDROID_NDK}/platforms/android-${CMAKE_SYSTEM_VERSION}/arch-${ANDROID_ABI}/usr/lib/liblog.so)

if (${WITH_SRV} STREQUAL ON)
	add_definitions("-DWITH_SRV")
endif (${WITH_SRV} STREQUAL ON)

add_library(mqtt SHARED sub_client.c mqtt_android.c)

target_link_libraries(mqtt libmosquitto msgpackc clientcore ${LOG_LIBRARIES})