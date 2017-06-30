#-----------------------------------------------------------------------------
#
# TinyWorker
#
#-----------------------------------------------------------------------------

SET(Worker_Header
        TinyBlockingQueue.h
        TinyWorker.h
    )

SET(Worker_Source
        TinyBlockingQueue.c
        TinyWorker.c
    )

SET(SRC
        ${Worker_Header}
        ${Worker_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_worker_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_worker_static PROPERTIES OUTPUT_NAME "tiny_worker_static")
SET_TARGET_PROPERTIES(tiny_worker_static PROPERTIES LINKER_LANGUAGE C)