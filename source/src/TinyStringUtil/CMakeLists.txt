#-----------------------------------------------------------------------------
#
# TinyStringUtil
#
#-----------------------------------------------------------------------------

SET(String_Header
        tiny_char_util.h
        tiny_str_equal.h
        tiny_str_split.h
        tiny_url_split.h
        tiny_buffer_append.h
        tiny_strnlen.h
        )

SET(String_Source
        tiny_char_util.c
        tiny_str_equal.c
        tiny_str_split.c
        tiny_url_split.c
        tiny_buffer_append.c
        tiny_strnlen.c
        )

SET(SRC
        ${String_Header}
        ${String_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_string_util_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_string_util_static PROPERTIES OUTPUT_NAME "tiny_string_util_static")
SET_TARGET_PROPERTIES(tiny_string_util_static PROPERTIES LINKER_LANGUAGE C)