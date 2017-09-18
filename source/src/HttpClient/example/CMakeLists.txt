#-----------------------------------------------------------------------------
#
# examples
#
#-----------------------------------------------------------------------------
ADD_EXECUTABLE(example_http_get example_http_get.c)
SET_TARGET_PROPERTIES(example_http_get PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(example_http_get
        http_client_static
        netty_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})