#-----------------------------------------------------------------------------
#
# TinyThread
#
#-----------------------------------------------------------------------------

SET(Thread_Header
        TinyCondition.h
        TinyMutex.h
        TinySemaphore.h
        TinyThread.h
    )

SET(Thread_Source
        TinyCondition.c
        TinyMutex.c
        TinySemaphore.c
        TinyThread.c
    )

SET(SRC
        ${Thread_Header}
        ${Thread_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_thread_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_thread_static PROPERTIES OUTPUT_NAME "tiny_thread_static")
SET_TARGET_PROPERTIES(tiny_thread_static PROPERTIES LINKER_LANGUAGE C)