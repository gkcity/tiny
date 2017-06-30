ADD_EXECUTABLE(tiny_malloc_test test.c)
TARGET_LINK_LIBRARIES(tiny_malloc_test tiny_malloc_static ${OS_LIB})