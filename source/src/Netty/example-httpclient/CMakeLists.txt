#-----------------------------------------------------------------------------
#
# example-httpclient
#
#-----------------------------------------------------------------------------

SET(SRC MyClientHandler.h
        MyClientHandler.c
        sample_http_client.c
        )

ADD_EXECUTABLE(sample_http_client ${SRC})
TARGET_LINK_LIBRARIES(sample_http_client
        netty_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB}
    )