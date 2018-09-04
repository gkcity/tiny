#-----------------------------------------------------------------------------
#
# example-ssdp
#
#-----------------------------------------------------------------------------

SET(SRC ExampleSsdpHandler.h
        ExampleSsdpHandler.c
        sample_ssdp.c
        )

ADD_EXECUTABLE(sample_ssdp ${SRC})
TARGET_LINK_LIBRARIES(sample_ssdp
        netty_static
        byte_buffer_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB}
    )