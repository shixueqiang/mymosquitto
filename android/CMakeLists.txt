include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/lib
			${STDBOOL_H_PATH} ${STDINT_H_PATH})
link_directories(${mosquitto_BINARY_DIR}/lib)

set(shared_src client_shared.c client_shared.h)
set(LOG_LIBRARIES ${CMAKE_ANDROID_NDK}/platforms/android-${CMAKE_SYSTEM_VERSION}/arch-${ANDROID_ABI}/usr/lib/liblog.so)

if (${WITH_SRV} STREQUAL ON)
	add_definitions("-DWITH_SRV")
endif (${WITH_SRV} STREQUAL ON)

add_library(mqtt SHARED sub_client.c sub_client_output.c pub_client.c client_android.c ${shared_src})

target_link_libraries(mqtt libmosquitto ${LOG_LIBRARIES})