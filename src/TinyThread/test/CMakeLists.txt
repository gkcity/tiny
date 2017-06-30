ADD_EXECUTABLE(test_thread test.c)
TARGET_LINK_LIBRARIES(test_thread  sc_thread_static 
                                   sc_memory_static
                                   sc_common_static
                                   ${OS_LIB})
