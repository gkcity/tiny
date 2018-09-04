ADD_EXECUTABLE(test_json_encode test_json_encode.c)
TARGET_LINK_LIBRARIES(test_json_encode
        json_static
        byte_buffer_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})

ADD_EXECUTABLE(test_json_decode test_json_decode.c)
TARGET_LINK_LIBRARIES(test_json_decode
        json_static
        byte_buffer_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})

ADD_EXECUTABLE(test_json_tokenizer test_json_tokenizer.c)
TARGET_LINK_LIBRARIES(test_json_tokenizer
        json_static
        byte_buffer_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})

