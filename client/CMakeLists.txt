include_directories(${mosquitto_SOURCE_DIR} ${mosquitto_SOURCE_DIR}/lib ${mosquitto_SOURCE_DIR}/clientcore
			${STDBOOL_H_PATH} ${STDINT_H_PATH} ${PTHREAD_INCLUDE_DIR})
link_directories(${mosquitto_BINARY_DIR}/lib)

if (${WITH_SRV} STREQUAL ON)
	add_definitions("-DWITH_SRV")
endif (${WITH_SRV} STREQUAL ON)

add_executable(mosquitto_pub pub_client.c)
add_executable(mosquitto_sub sub_client.c)


target_link_libraries(mosquitto_pub libmosquitto msgpackc clientcore)
target_link_libraries(mosquitto_sub libmosquitto msgpackc clientcore)

if (QNX)
    target_link_libraries(mosquitto_pub socket)
    target_link_libraries(mosquitto_sub socket)
endif()

install(TARGETS mosquitto_pub RUNTIME DESTINATION "${BINDIR}" LIBRARY DESTINATION "${LIBDIR}")
install(TARGETS mosquitto_sub RUNTIME DESTINATION "${BINDIR}" LIBRARY DESTINATION "${LIBDIR}")
