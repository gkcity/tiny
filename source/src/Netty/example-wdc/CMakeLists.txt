#-----------------------------------------------------------------------------
#
# example-wdc
#
#-----------------------------------------------------------------------------

SET(SRC WdcHandler.h
        WdcHandler.c
        sample_wdc_server.c
        )

ADD_EXECUTABLE(sample_wdc_server ${SRC})
TARGET_LINK_LIBRARIES(sample_wdc_server
        netty_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB}
    )