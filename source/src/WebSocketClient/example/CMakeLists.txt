#-----------------------------------------------------------------------------
#
# examples
#
#-----------------------------------------------------------------------------
ADD_EXECUTABLE(example_ws_client example_ws_client.c)
SET_TARGET_PROPERTIES(example_ws_client PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(example_ws_client
        json_static
        ws_client_static
        netty_static
        tiny_buffer_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})