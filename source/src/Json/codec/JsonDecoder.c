/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonDecoder.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "JsonDecoder.h"
#include "value/JsonNumber.h"
#include "value/JsonString.h"
#include "JsonArray.h"

#define TAG     "JsonDecoder"


typedef enum _TokenAnalystResult
{
    TOKEN_ANALYSIS_OK                                = 0,
    TOKEN_ANALYSIS_E_OBJECT_START_NOT_FOUND          = 1,
    TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND            = 2,
    TOKEN_ANALYSIS_E_OBJECT_KEY_NOT_FOUND            = 3,
    TOKEN_ANALYSIS_E_OBJECT_KEY_TOO_LONG             = 4,
    TOKEN_ANALYSIS_E_OBJECT_COLON_NOT_FOUND          = 5,
    TOKEN_ANALYSIS_E_OBJECT_VALUE_NOT_FOUND          = 6,
    TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID            = 7,
    TOKEN_ANALYSIS_E_OBJECT_END_UNNECESSARY_TOKEN    = 8,
    TOKEN_ANALYSIS_E_ARRAY_VALUE_INVALID             = 9,
    TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND             = 10,

} TokenAnalystResult;

TINY_LOR
static JsonObject * JsonDecoder_DecodeObject(JsonDecoder *thiz);

TINY_LOR
static JsonArray * JsonDecoder_DecodeArray(JsonDecoder *thiz);

TINY_LOR
static JsonString * JsonDecoder_DecodeString(JsonDecoder *thiz, JsonToken *token);

TINY_LOR
static JsonNumber * JsonDecoder_DecodeNumber(JsonDecoder *thiz, JsonToken *token);

TINY_LOR
TinyRet JsonDecoder_Construct(JsonDecoder *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(JsonDecoder));
    ret = JsonTokenizer_Construct(&thiz->tokenizer);

    return ret;
}

TINY_LOR
void JsonDecoder_Dispose(JsonDecoder *thiz)
{
    RETURN_IF_FAIL(thiz);

    JsonTokenizer_Dispose(&thiz->tokenizer);
}

TINY_LOR
static JsonValue * JsonDecoder_DecodeValue(JsonDecoder *thiz)
{
    JsonToken *token = JsonTokenizer_Head(&thiz->tokenizer);
    JsonValue *value = NULL;

    switch (token->type)
    {
        case JSON_TOKEN_OBJECT_START:
            value = JsonValue_NewValue(JSON_OBJECT, JsonDecoder_DecodeObject(thiz));
            break;

        case JSON_TOKEN_OBJECT_END:
            break;

        case JSON_TOKEN_ARRAY_START:
            value = JsonValue_NewValue(JSON_ARRAY, JsonDecoder_DecodeArray(thiz));
            break;

        case JSON_TOKEN_ARRAY_END:
            break;

        case JSON_TOKEN_COMMA:
            break;

        case JSON_TOKEN_COLON:
            break;

        case JSON_TOKEN_NULL:
            value = JsonValue_NewNull();
            JsonTokenizer_Pop(&thiz->tokenizer);
            break;

        case JSON_TOKEN_TRUE:
            value = JsonValue_NewBoolean(true);
            JsonTokenizer_Pop(&thiz->tokenizer);
            break;

        case JSON_TOKEN_FALSE:
            value = JsonValue_NewBoolean(false);
            JsonTokenizer_Pop(&thiz->tokenizer);
            break;

        case JSON_TOKEN_STRING:
            value = JsonValue_NewValue(JSON_STRING, JsonDecoder_DecodeString(thiz, token));
            JsonTokenizer_Pop(&thiz->tokenizer);
            break;

        case JSON_TOKEN_NUMBER:
            value = JsonValue_NewValue(JSON_NUMBER, JsonDecoder_DecodeNumber(thiz, token));
            JsonTokenizer_Pop(&thiz->tokenizer);
            break;
    }

    return value;
}

TINY_LOR
static JsonArray * JsonDecoder_DecodeArray(JsonDecoder *thiz)
{
    JsonArray * array = NULL;

    do
    {
        TokenAnalystResult result = TOKEN_ANALYSIS_OK;
        JsonToken *token = JsonTokenizer_Head(&thiz->tokenizer);
        if (token == NULL)
        {
            LOG_D(TAG, "JsonArray start token not found!");
            break;
        }

        if (token->type != JSON_TOKEN_ARRAY_START)
        {
            LOG_D(TAG, "JsonArray start token invalid: %d", token->type);
            break;
        }

        array = JsonArray_New();
        if (array == NULL)
        {
            break;
        }

        JsonTokenizer_Pop(&thiz->tokenizer);

        while (true)
        {
            JsonValue *value = NULL;

            // ]
            token = JsonTokenizer_Head(&thiz->tokenizer);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_ARRAY_END)
            {
                JsonTokenizer_Pop(&thiz->tokenizer);
                break;
            }

            // value
            value = JsonDecoder_DecodeValue(thiz);
            if (value == NULL)
            {
                result = TOKEN_ANALYSIS_E_ARRAY_VALUE_INVALID;
                break;
            }

            JsonArray_AddValue(array, value);

            // , or ]
            token = JsonTokenizer_Head(&thiz->tokenizer);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_COMMA)
            {
                JsonTokenizer_Pop(&thiz->tokenizer);
                continue;
            }

            if (token->type == JSON_TOKEN_ARRAY_END)
            {
                JsonTokenizer_Pop(&thiz->tokenizer);
                break;
            }

            result = TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND;
            break;
        }

        if (result != TOKEN_ANALYSIS_OK)
        {
            JsonArray_Delete(array);
            array = NULL;
            break;
        }
    } while (false);

    return array;
}

TINY_LOR
static JsonString * JsonDecoder_DecodeString(JsonDecoder *thiz, JsonToken *token)
{
    JsonString * string = NULL;

    do
    {
        string = JsonString_New(NULL);
        if (string == NULL)
        {
            break;
        }

        string->value = tiny_malloc(token->length - 1);
        if (string->value == NULL)
        {
            JsonString_Delete(string);
            string = NULL;
            break;
        }

        memset(string->value, 0, token->length - 1);
        memcpy(string->value, thiz->tokenizer.string + token->offset + 1, token->length - 2);
        string->length = token->length;
    } while (false);

    return string;
}

TINY_LOR
static JsonNumber * JsonDecoder_DecodeNumber(JsonDecoder *thiz, JsonToken *token)
{
    JsonNumber * number = NULL;
    
    do
    {
        char value[128];
        char *stop = NULL;

        number = JsonNumber_NewInteger(0);
        if (number == NULL)
        {
            break;
        }

        if (token->length >= 128)
        {
            JsonNumber_Delete(number);
            number = NULL;
            break;
        }

        memset(value, 0, 128);
        memset(value, 0, token->length + 1);
        memcpy(value, thiz->tokenizer.string + token->offset, token->length);

        if (strstr(value, ".") == NULL)
        {
            number->value.intValue = strtol(value, &stop, 10);
        }
        else
        {
            number->type = JSON_NUMBER_FLOAT;
            number->value.floatValue = strtof(value, &stop);
        }
    } while (false);

    return number;
}

TINY_LOR
static JsonObject * JsonDecoder_DecodeObject(JsonDecoder *thiz)
{
    JsonObject *object = NULL;

    do
    {
        TokenAnalystResult result = TOKEN_ANALYSIS_OK;

        JsonToken *token = JsonTokenizer_Head(&thiz->tokenizer);
        if (token == NULL)
        {
            LOG_D(TAG, "JsonObject start token not found!");
            break;
        }

        if (token->type != JSON_TOKEN_OBJECT_START)
        {
            LOG_D(TAG, "JsonObject start token invalid: %d", token->type);
            break;
        }

        object = JsonObject_New();
        if (object == NULL)
        {
            LOG_D(TAG, "JsonObject_New FAILED!");
            break;
        }

        JsonTokenizer_Pop(&thiz->tokenizer);

        while (true)
        {
            char key[128];
            JsonValue *value = NULL;

            // }
            token = JsonTokenizer_Head(&thiz->tokenizer);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_OBJECT_END) 
            {
                JsonTokenizer_Pop(&thiz->tokenizer);
                break;
            }

            // key
            if (token->type != JSON_TOKEN_STRING)
            {
                LOG_D(TAG, "key type invalid: %d", token->type);
                result = TOKEN_ANALYSIS_E_OBJECT_KEY_NOT_FOUND;
                break;
            }
            
            if ((token->length - 2) > 127)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_KEY_TOO_LONG;
                break;
            }

            memset(key, 0, 128);
            memcpy(key, thiz->tokenizer.string + token->offset + 1, token->length - 2);

            JsonTokenizer_Pop(&thiz->tokenizer);

            // :
            token = JsonTokenizer_Head(&thiz->tokenizer);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_COLON_NOT_FOUND;
                break;
            }

            if (token->type != JSON_TOKEN_COLON)
            {
                LOG_D(TAG, "':' type invalid: %d", token->type);
                result = TOKEN_ANALYSIS_E_OBJECT_COLON_NOT_FOUND;
                break;
            }

            JsonTokenizer_Pop(&thiz->tokenizer);

            // value
            value = JsonDecoder_DecodeValue(thiz);
            if (value == NULL)
            {
                LOG_D(TAG, "JsonValue invalid: %s", key);
                result = TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID;
                break;
            }

            if (RET_FAILED(JsonObject_PutValue(object, key, value))) 
            {
                LOG_D(TAG, "JsonObject_PutValue failed");
                result = TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID;
                break;
            }

            // , or }
            token = JsonTokenizer_Head(&thiz->tokenizer);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_COMMA)
            {
                JsonTokenizer_Pop(&thiz->tokenizer);
                continue;
            }

            if (token->type == JSON_TOKEN_OBJECT_END)
            {
                JsonTokenizer_Pop(&thiz->tokenizer);
                break;
            }

            result = TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND;
            break;
        };

        if (result != TOKEN_ANALYSIS_OK)
        {
            JsonObject_Delete(object);
            object = NULL;
            break;
        }
    } while (0);

    return object;
}

TINY_LOR
JsonObject * JsonDecoder_Parse(JsonDecoder *thiz, const char *string, JsonDecoderMode mode)
{
    JsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(string, NULL);

    do
    {
        if (RET_FAILED(JsonTokenizer_Parse(&thiz->tokenizer, string, mode == JSON_DECODE_NORMAL)))
        {
            break;
        }

        object = JsonDecoder_DecodeObject(thiz);
        if (object == NULL)
        {
            break;
        }

        if (JsonTokenizer_Head(&thiz->tokenizer) != NULL)
        {
            JsonObject_Delete(object);
            object = NULL;
        }
    } while (false);

    return object;
}