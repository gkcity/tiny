/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonDecoderCompact.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "JsonDecoderCompact.h"
#include "value/JsonNumber.h"
#include "value/JsonString.h"
#include "JsonArray.h"

#define TAG     "JsonDecoderCompact"


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
static JsonObject * JsonDecoderCompact_PeakObject(JsonDecoderCompact *thiz);

TINY_LOR
static JsonArray * JsonDecoderCompact_PeakArray(JsonDecoderCompact *thiz);

TINY_LOR
static JsonString * JsonDecoderCompact_PeakString(JsonDecoderCompact *thiz, JsonToken *token);

TINY_LOR
static JsonNumber * JsonDecoderCompact_PeakNumber(JsonDecoderCompact *thiz, JsonToken *token);

TINY_LOR
TinyRet JsonDecoderCompact_Construct(JsonDecoderCompact *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(JsonDecoderCompact));
    ret = JsonTokenizer_Construct(&thiz->tokenizer);

    return ret;
}

TINY_LOR
void JsonDecoderCompact_Dispose(JsonDecoderCompact *thiz)
{
    RETURN_IF_FAIL(thiz);

    JsonTokenizer_Dispose(&thiz->tokenizer);
}

TINY_LOR
static JsonValue * JsonDecoderCompact_PeakValue(JsonDecoderCompact *thiz)
{
    JsonToken *token = JsonTokenizer_Next(&thiz->tokenizer);
//    JsonToken *token = TinyList_GetAt(&thiz->tokens, thiz->index);
    JsonValue *value = NULL;

    switch (token->type)
    {
        case JSON_TOKEN_OBJECT_START:
            value = JsonValue_NewValue(JSON_OBJECT, JsonDecoderCompact_PeakObject(thiz));
            break;

        case JSON_TOKEN_OBJECT_END:
            break;

        case JSON_TOKEN_ARRAY_START:
            value = JsonValue_NewValue(JSON_ARRAY, JsonDecoderCompact_PeakArray(thiz));
            break;

        case JSON_TOKEN_ARRAY_END:
            break;

        case JSON_TOKEN_COMMA:
            break;

        case JSON_TOKEN_COLON:
            break;

        case JSON_TOKEN_NULL:
            value = JsonValue_NewNull();
//            thiz->index++;
            break;

        case JSON_TOKEN_TRUE:
            value = JsonValue_NewBoolean(true);
//            thiz->index++;
            break;

        case JSON_TOKEN_FALSE:
            value = JsonValue_NewBoolean(false);
//            thiz->index++;
            break;

        case JSON_TOKEN_STRING:
            value = JsonValue_NewValue(JSON_STRING, JsonDecoderCompact_PeakString(thiz, token));
//            thiz->index++;
            break;

        case JSON_TOKEN_NUMBER:
            value = JsonValue_NewValue(JSON_NUMBER, JsonDecoderCompact_PeakNumber(thiz, token));
//            thiz->index++;
            break;
    }

    return value;
}

TINY_LOR
static JsonArray * JsonDecoderCompact_PeakArray(JsonDecoderCompact *thiz)
{
    JsonArray * array = NULL;

    do
    {
        TokenAnalystResult result = TOKEN_ANALYSIS_OK;
        JsonToken *token = JsonTokenizer_Next(&thiz->tokenizer);
//        JsonToken *token = TinyList_GetAt(&thiz->tokens, thiz->index++);
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

        array = JsonArray_New(JSON_UNDEFINED);
        if (array == NULL)
        {
            break;
        }

        while (true)
        {
            JsonValue *value = NULL;

            // ]
            token = JsonTokenizer_Next(&thiz->tokenizer);
//            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL) 
            {
                result = TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_ARRAY_END)
            {
//                thiz->index++;
                break;
            }

            // value
            value = JsonDecoderCompact_PeakValue(thiz);
            if (value == NULL)
            {
                result = TOKEN_ANALYSIS_E_ARRAY_VALUE_INVALID;
                break;
            }

            if (array->values.size == 0) 
            {
                array->type = value->type;
            }

            JsonArray_AddValue(array, value);

            // , or ]
            token = JsonTokenizer_Next(&thiz->tokenizer);
//            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_COMMA)
            {
//                thiz->index++;
                continue;
            }

            if (token->type == JSON_TOKEN_ARRAY_END)
            {
//                thiz->index++;
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
static JsonString * JsonDecoderCompact_PeakString(JsonDecoderCompact *thiz, JsonToken *token)
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
static JsonNumber * JsonDecoderCompact_PeakNumber(JsonDecoderCompact *thiz, JsonToken *token)
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
static JsonObject * JsonDecoderCompact_PeakObject(JsonDecoderCompact *thiz)
{
    JsonObject *object = NULL;

    do
    {
        TokenAnalystResult result = TOKEN_ANALYSIS_OK;

        JsonToken *token = JsonTokenizer_Next(&thiz->tokenizer);

//        JsonToken *token = TinyList_GetAt(&thiz->tokens, thiz->index++);
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

        while (true)
        {
            char key[128];
            JsonValue *value = NULL;

            // }
            token = JsonTokenizer_Next(&thiz->tokenizer);
//            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_OBJECT_END) 
            {
//                thiz->index++;
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

//            thiz->index++;

            // :
            token = JsonTokenizer_Next(&thiz->tokenizer);
//            token = TinyList_GetAt(&thiz->tokens, thiz->index);
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

//            thiz->index++;

            // value
            value = JsonDecoderCompact_PeakValue(thiz);
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
            token = JsonTokenizer_Next(&thiz->tokenizer);
//            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_COMMA)
            {
//                thiz->index++;
                continue;
            }

            if (token->type == JSON_TOKEN_OBJECT_END)
            {
//                thiz->index++;
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
JsonObject * JsonDecoderCompact_Parse(JsonDecoderCompact *thiz, const char *string)
{
    JsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(string, NULL);

    do
    {
        if (RET_FAILED(JsonTokenizer_Parse(&thiz->tokenizer, string, false)))
        {
            break;
        }

        object = JsonDecoderCompact_PeakObject(thiz);
        if (object == NULL)
        {
            break;
        }

        if (JsonTokenizer_Next(&thiz->tokenizer) != NULL)
        {
            JsonObject_Delete(object);
            object = NULL;
        }
    } while (false);

    return object;
}