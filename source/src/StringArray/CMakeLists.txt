#-----------------------------------------------------------------------------
#
# StringArray
#
#-----------------------------------------------------------------------------

SET(StringArray_Header
        StringArray.h
        )

SET(StringArray_Source
        StringArray.c
        )

SET(SRC
        ${StringArray_Header}
        ${StringArray_Source}
        )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(string_array_static STATIC ${SRC})
SET_TARGET_PROPERTIES(string_array_static PROPERTIES OUTPUT_NAME "string_array_static")
SET_TARGET_PROPERTIES(string_array_static PROPERTIES LINKER_LANGUAGE C)