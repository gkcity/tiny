#-----------------------------------------------------------------------------
#
# TinyLog
#
#-----------------------------------------------------------------------------


SET(Header
        tiny_log.h
        tiny_log_print.h
        tiny_log_binary.h
    )

SET(Source
        tiny_log_print.c
        tiny_log_binary.c
    )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_log_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_log_static PROPERTIES OUTPUT_NAME "tiny_log_static")
SET_TARGET_PROPERTIES(tiny_log_static PROPERTIES LINKER_LANGUAGE C)