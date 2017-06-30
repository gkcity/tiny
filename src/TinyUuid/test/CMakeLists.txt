ADD_EXECUTABLE(test_uuid test.c)
TARGET_LINK_LIBRARIES(test_uuid sc_uuid_static 
                                sc_memory_static
                                sc_common_static
                                ${OS_LIB})
