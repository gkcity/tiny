ADD_EXECUTABLE(test_json_encoder test_json_encoder.c)
TARGET_LINK_LIBRARIES(test_json_encoder
        json_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})

ADD_EXECUTABLE(test_json_decoder test_json_decoder.c)
TARGET_LINK_LIBRARIES(test_json_decoder
        json_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})

ADD_EXECUTABLE(test_json_tokenizer test_json_tokenizer.c)
TARGET_LINK_LIBRARIES(test_json_tokenizer
        json_static
        tiny_string_util_static
        tiny_container_static
        tiny_log_static
        tiny_portable_static
        tiny_base_static
        ${OS_LIB})

