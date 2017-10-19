ADD_EXECUTABLE(test_http_message test_http_message.c)
TARGET_LINK_LIBRARIES(test_http_message
        netty_static
        tiny_buffer_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})
