ADD_EXECUTABLE(test_tiny_list test.c)
TARGET_LINK_LIBRARIES(test_tiny_list
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})