include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/lib ${mosquitto_SOURCE_DIR}/clientcore
			${STDBOOL_H_PATH} ${STDINT_H_PATH} ${PTHREAD_INCLUDE_DIR})
link_directories(${mosquitto_BINARY_DIR}/lib)

if (${WITH_SRV} STREQUAL ON)
	add_definitions("-DWITH_SRV")
endif (${WITH_SRV} STREQUAL ON)

add_library(pushmqtt SHARED sub_client.c)

target_link_libraries(pushmqtt libmosquitto msgpackc clientcore ${PTHREAD_LIBRARIES} dl)

if (QNX)
    target_link_libraries(mosquitto_pub socket)
    target_link_libraries(mosquitto_sub socket)
endif()
