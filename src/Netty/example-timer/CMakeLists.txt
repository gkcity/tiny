#-----------------------------------------------------------------------------
#
# example-timer
#
#-----------------------------------------------------------------------------

SET(SRC sample_timer.c)

ADD_EXECUTABLE(sample_timer ${SRC})
TARGET_LINK_LIBRARIES(sample_timer
        netty_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})