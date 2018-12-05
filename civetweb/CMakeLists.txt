include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/lib ${mosquitto_SOURCE_DIR}/civetweb/include
			${STDBOOL_H_PATH} ${STDINT_H_PATH} ${PTHREAD_INCLUDE_DIR})
link_directories(${mosquitto_BINARY_DIR}/lib)

set(shared_src client_shared.c client_shared.h mqtt_main.h)

if (${WITH_SRV} STREQUAL ON)
	add_definitions("-DWITH_SRV")
endif (${WITH_SRV} STREQUAL ON)

add_library(pushmqtt SHARED sub_client.c sub_client_output.c ${shared_src})

target_link_libraries(pushmqtt libmosquitto msgpackc ${PTHREAD_LIBRARIES} dl)

if (QNX)
    target_link_libraries(mosquitto_pub socket)
    target_link_libraries(mosquitto_sub socket)
endif()
