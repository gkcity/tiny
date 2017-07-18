INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/miot)
ADD_EXECUTABLE(test_miot test_miot.c)
TARGET_LINK_LIBRARIES(test_miot
	miot_static
        netty_static
        tiny_json_static
        tiny_string_util_static
        tiny_container_static
        tiny_portable_static
        tiny_log_static
        tiny_base_static
        ${OS_LIB})
