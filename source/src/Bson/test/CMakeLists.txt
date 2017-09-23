ADD_EXECUTABLE(test_bson_encode test_bson_encode.c)
TARGET_LINK_LIBRARIES(test_bson_encode
        bson_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})

ADD_EXECUTABLE(test_bson_decode test_bson_decode.c)
TARGET_LINK_LIBRARIES(test_bson_decode
        bson_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})