include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/lib
			${STDBOOL_H_PATH} ${STDINT_H_PATH} ${PTHREAD_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
link_directories(${mosquitto_BINARY_DIR}/lib)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(shared_src client_shared.c client_shared.h)

if (${WITH_SRV} STREQUAL ON)
	add_definitions("-DWITH_SRV")
endif (${WITH_SRV} STREQUAL ON)

add_library(clientcore STATIC sub_client_output.c ${shared_src})

target_link_libraries(clientcore libmosquitto msgpackc)

if (QNX)
    target_link_libraries(mosquitto_pub socket)
    target_link_libraries(mosquitto_sub socket)
endif()
