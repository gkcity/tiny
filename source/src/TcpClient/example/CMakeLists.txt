#-----------------------------------------------------------------------------
#
# examples
#
#-----------------------------------------------------------------------------
ADD_EXECUTABLE(example_tcp_client example_tcp_client.c)
SET_TARGET_PROPERTIES(example_tcp_client PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(example_tcp_client
        tcp_client_static
        netty_static
        byte_buffer_static
        json_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})