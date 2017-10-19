#-----------------------------------------------------------------------------
#
# example-httpserver
#
#-----------------------------------------------------------------------------

SET(SRC ExampleHandler.h
        ExampleHandler.c
        sample_http_server.c
        )

ADD_EXECUTABLE(sample_http_server ${SRC})
TARGET_LINK_LIBRARIES(sample_http_server
        netty_static
        tiny_buffer_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB}
    )