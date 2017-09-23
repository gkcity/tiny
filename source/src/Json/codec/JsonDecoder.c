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

#include <ctype.h>
#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_char_util.h>
#include "JsonDecoder.h"
#include "JsonToken.h"
#include "value/JsonNumber.h"
#include "value/JsonString.h"
#include "JsonArray.h"

#define TAG     "JsonDecoder"

typedef enum _TokenParseResult
{
    TOKEN_PARSE_OK                      = 0,
    TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY    = 1,
    TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED  = 2,
    TOKEN_PARSE_E_INVALID_VALUE         = 3,
    TOKEN_PARSE_E_EXPECT_VALUE          = 4,
    TOKEN_PARSE_E_INVALID_STRING        = 5,
    TOKEN_PARSE_E_INVALID_NUMBER        = 6,
} TokenParseResult;

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
static JsonObject * JsonDecoder_PeakObject(JsonDecoder *thiz);

TINY_LOR
static JsonArray * JsonDecoder_PeakArray(JsonDecoder *thiz);

TINY_LOR
static JsonString * JsonDecoder_PeakString(JsonDecoder *thiz, JsonToken *token);

TINY_LOR
static JsonNumber * JsonDecoder_PeakNumber(JsonDecoder *thiz, JsonToken *token);

TINY_LOR
static void _OnTokenDelete (void * data, void *ctx)
{
    tiny_free(data);
}

TINY_LOR
TinyRet JsonDecoder_Construct(JsonDecoder *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    ret = TinyList_Construct(&thiz->tokens);
    if (RET_SUCCEEDED(ret))
    {
        TinyList_SetDeleteListener(&thiz->tokens, _OnTokenDelete, thiz);
    }

    return ret;
}

TINY_LOR
void JsonDecoder_Dispose(JsonDecoder *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyList_Dispose(&thiz->tokens);
}

TINY_LOR
static TokenParseResult JsonDecoder_ParseTokenMonocase(JsonDecoder *thiz, JsonTokenType type)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    do
    {
        uint32_t offset = (uint32_t) (thiz->current - thiz->string);

        JsonToken * token = JsonToken_New(type, offset, 1);;
        if (token == NULL)
        {
            result = TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY;
            break;
        }

        if (RET_FAILED(TinyList_AddTail(&thiz->tokens, token)))
        {
            result = TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED;
            break;
        }

        thiz->index++;
        thiz->current ++;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonDecoder_ParseNull(JsonDecoder *thiz)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    do
    {
        uint32_t offset = (uint32_t) (thiz->current - thiz->string);

        if (thiz->current[0] != 'n' ||
            thiz->current[1] != 'u' ||
            thiz->current[2] != 'l' ||
            thiz->current[3] != 'l')
        {
            result = TOKEN_PARSE_E_EXPECT_VALUE;
            break;
        }

        JsonToken * token = JsonToken_New(JSON_TOKEN_NULL, offset, 4);;
        if (token == NULL)
        {
            return TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY;
        }

        if (RET_FAILED(TinyList_AddTail(&thiz->tokens, token)))
        {
            return TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED;
        }

        thiz->index += 4;
        thiz->current += 4;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonDecoder_ParseTrue(JsonDecoder *thiz)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    do
    {
        uint32_t offset = (uint32_t) (thiz->current - thiz->string);

        if (thiz->current[0] != 't' ||
            thiz->current[1] != 'r' ||
            thiz->current[2] != 'u' ||
            thiz->current[3] != 'e')
        {
            result = TOKEN_PARSE_E_EXPECT_VALUE;
            break;
        }

        JsonToken * token = JsonToken_New(JSON_TOKEN_TRUE, offset, 4);;
        if (token == NULL)
        {
            return TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY;
        }

        if (RET_FAILED(TinyList_AddTail(&thiz->tokens, token)))
        {
            return TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED;
        }

        thiz->index += 4;
        thiz->current += 4;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonDecoder_ParseFalse(JsonDecoder *thiz)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    do
    {
        uint32_t offset = (uint32_t) (thiz->current - thiz->string);

        if (thiz->current[0] != 'f' ||
            thiz->current[1] != 'a' ||
            thiz->current[2] != 'l' ||
            thiz->current[3] != 's' ||
            thiz->current[4] != 'e')
        {
            result = TOKEN_PARSE_E_EXPECT_VALUE;
            break;
        }

        JsonToken * token = JsonToken_New(JSON_TOKEN_FALSE, offset, 5);;
        if (token == NULL)
        {
            return TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY;
        }

        if (RET_FAILED(TinyList_AddTail(&thiz->tokens, token)))
        {
            return TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED;
        }

        thiz->index += 5;
        thiz->current += 5;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonDecoder_ParseString(JsonDecoder *thiz)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    do
    {
        uint32_t offset = (uint32_t) (thiz->current - thiz->string);
        const char *start = thiz->current;

        thiz->index ++;
        thiz->current ++;

        while (true)
        {
            char c = *thiz->current;

            if (c == '\\')
            {
                thiz->index ++;
                thiz->current ++;

                c = *thiz->current;

                switch (c)
                {
                    case '"':
                    case '\\':
                    case '/':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 't':
                        thiz->index ++;
                        thiz->current ++;
                        break;

                    case 'u':
                        thiz->index ++;
                        thiz->current ++;

                        if (is_hex(thiz->current[0]) && is_hex(thiz->current[1]) && is_hex(thiz->current[2]) && is_hex(thiz->current[3]))
                        {
                            thiz->index += 4;
                            thiz->current += 4;
                        }
                        else
                        {
                            result = TOKEN_PARSE_E_INVALID_STRING;
                        }
                        break;

                    default:
                        result = TOKEN_PARSE_E_INVALID_STRING;
                        break;
                }
            }

            else if (c == '"')
            {
                thiz->index ++;
                thiz->current ++;
                break;
            }

            else if (isprint(c))
            {
                thiz->index++;
                thiz->current ++;
            }

            else 
            {
                result = TOKEN_PARSE_E_INVALID_STRING;
            }

            if (result != TOKEN_PARSE_OK)
            {
                break;
            }
        }

        if (result != TOKEN_PARSE_OK)
        {
            break;
        }

        JsonToken * token = JsonToken_New(JSON_TOKEN_STRING, offset, (uint32_t)(thiz->current - start));
        if (token == NULL)
        {
            result = TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY;
            break;
        }

        if (RET_FAILED(TinyList_AddTail(&thiz->tokens, token)))
        {
            result = TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED;
            break;
        }
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonDecoder_ParseNumber(JsonDecoder *thiz)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    do
    {
        uint32_t offset = (uint32_t)(thiz->current - thiz->string);
        const char *start = thiz->current;
        char c = *thiz->current;

        do
        {
            if (c == '-')
            {
                thiz->index ++;
                thiz->current ++;
                c = *thiz->current;
            }

            if (c == '0')
            {
                thiz->index ++;
                thiz->current ++;
                c = *thiz->current;
            }
            else if (c >= '1' && c <= '9')
            {
                thiz->index ++;
                thiz->current ++;
                c = *thiz->current;

                while (isdigit(c))
                {
                    thiz->index ++;
                    thiz->current ++;
                    c = *thiz->current;
                }
            }
            else 
            {
                result = TOKEN_PARSE_E_INVALID_NUMBER;
                break;
            }

            if (c == '.')
            {
                thiz->index ++;
                thiz->current ++;
                c = *thiz->current;
            }
            else if (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '}' || c == ']' || c == ',')
            {
                break;
            }
            else 
            {
                result = TOKEN_PARSE_E_INVALID_NUMBER;
                break;
            }

            while (isdigit(c))
            {
                thiz->index ++;
                thiz->current ++;
                c = *thiz->current;
            }

            if (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '}' || c == ']')
            {
                break;
            }
            else if (c == 'e' || c == 'E')
            {
                thiz->index ++;
                thiz->current ++;
                c = *thiz->current;

                if (c == '+' || c == '-') 
                {
                    while (isdigit(c))
                    {
                        thiz->current++;
                        c = *thiz->current;
                    }

                    if (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '}' || c == ']')
                    {
                        break;
                    }
                    else 
                    {
                        result = TOKEN_PARSE_E_INVALID_NUMBER;
                        break;
                    }
                }
                else 
                {
                    result = TOKEN_PARSE_E_INVALID_NUMBER;
                    break;
                }
            }
            else 
            {
                result = TOKEN_PARSE_E_INVALID_NUMBER;
                break;
            }
        } while (false);

        if (result != TOKEN_PARSE_OK)
        {
            break;
        }

        JsonToken * token = JsonToken_New(JSON_TOKEN_NUMBER, offset, (uint32_t)(thiz->current - start));
        if (token == NULL)
        {
            result = TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY;
            break;
        }

        if (RET_FAILED(TinyList_AddTail(&thiz->tokens, token)))
        {
            result = TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED;
            break;
        }
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonDecoder_ParseToken(JsonDecoder *thiz)
{
    switch (*thiz->current)
    {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            thiz->index ++;
            thiz->current ++;
            return TOKEN_PARSE_OK;

        case '{':
            return JsonDecoder_ParseTokenMonocase(thiz, JSON_TOKEN_OBJECT_START);

        case '}':
            return JsonDecoder_ParseTokenMonocase(thiz, JSON_TOKEN_OBJECT_END);

        case '[':
            return JsonDecoder_ParseTokenMonocase(thiz, JSON_TOKEN_ARRAY_START);

        case ']':
            return JsonDecoder_ParseTokenMonocase(thiz, JSON_TOKEN_ARRAY_END);

        case ',':
            return JsonDecoder_ParseTokenMonocase(thiz, JSON_TOKEN_COMMA);

        case ':':
            return JsonDecoder_ParseTokenMonocase(thiz, JSON_TOKEN_COLON);

        case 'n':
            return JsonDecoder_ParseNull(thiz);

        case 't':
            return JsonDecoder_ParseTrue(thiz);

        case 'f':
            return JsonDecoder_ParseFalse(thiz);

        case '"':
            return JsonDecoder_ParseString(thiz);

        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return JsonDecoder_ParseNumber(thiz);

        default:
            return TOKEN_PARSE_E_INVALID_VALUE;
    }
}

TINY_LOR
TinyRet JsonDecoder_Parse(JsonDecoder *thiz, const char *string)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(string, TINY_RET_E_ARG_NULL);

    thiz->index = 0;
    thiz->string = string;
    thiz->current = string;

    while (*thiz->current != '\0')
    {
        result = JsonDecoder_ParseToken(thiz);
        if (result != TOKEN_PARSE_OK)
        {
            LOG_D(TAG, "JsonDecoder_ParseToken FAILED: %d, index: %d", result, thiz->index);
            break;
        }
    }

    return (result == TOKEN_PARSE_OK) ? TINY_RET_OK : TINY_RET_E_ARG_INVALID;
}

#ifdef TINY_DEBUG
const char * JsonToken_TypeToString(JsonTokenType type)
{
    switch(type)
    {
        case JSON_TOKEN_OBJECT_START:
            return "ObjectStart";

        case JSON_TOKEN_OBJECT_END:
            return "ObjectEnd  ";

        case JSON_TOKEN_ARRAY_START:
            return "ArrayStart ";

        case JSON_TOKEN_ARRAY_END:
            return "ArrayEnd   ";

        case JSON_TOKEN_COMMA:
            return "Comma      ";

        case JSON_TOKEN_COLON:
            return "Colon      ";

        case JSON_TOKEN_NULL:
            return "Null       ";

        case JSON_TOKEN_TRUE:
            return "True       ";

        case JSON_TOKEN_FALSE:
            return "False      ";

        case JSON_TOKEN_STRING:
            return "String     ";

        case JSON_TOKEN_NUMBER:
            return "Number     ";

        default:
            return "UNDEFINED  ";
    }
}

void JsonDecoder_Print(JsonDecoder *thiz)
{
    for (uint32_t i = 0; i < thiz->tokens.size; ++i)
    {
        JsonToken * token = TinyList_GetAt(&thiz->tokens, i);
        char buf[128];

        memset(buf, 0, 128);
        memcpy(buf, thiz->string + token->offset, token->length);
        printf("token [%d]: %s (%d, %d) -> %s\n", i, JsonToken_TypeToString(token->type), token->offset, token->length, buf);
    }

    printf("\n");
}
#endif

TINY_LOR
static JsonValue * JsonDecoder_PeakValue(JsonDecoder *thiz)
{
    JsonToken *token = TinyList_GetAt(&thiz->tokens, thiz->index);
    JsonValue *value = NULL;

    switch (token->type)
    {
        case JSON_TOKEN_OBJECT_START:
            value = JsonValue_NewValue(JSON_OBJECT, JsonDecoder_PeakObject(thiz));
            break;

        case JSON_TOKEN_OBJECT_END:
            break;

        case JSON_TOKEN_ARRAY_START:
            value = JsonValue_NewValue(JSON_ARRAY, JsonDecoder_PeakArray(thiz));
            break;

        case JSON_TOKEN_ARRAY_END:
            break;

        case JSON_TOKEN_COMMA:
            break;

        case JSON_TOKEN_COLON:
            break;

        case JSON_TOKEN_NULL:
            value = JsonValue_NewNull();
            thiz->index++;
            break;

        case JSON_TOKEN_TRUE:
            value = JsonValue_NewBoolean(true);
            thiz->index++;
            break;

        case JSON_TOKEN_FALSE:
            value = JsonValue_NewBoolean(false);
            thiz->index++;
            break;

        case JSON_TOKEN_STRING:
            value = JsonValue_NewValue(JSON_STRING, JsonDecoder_PeakString(thiz, token));
            thiz->index++;
            break;

        case JSON_TOKEN_NUMBER:
            value = JsonValue_NewValue(JSON_NUMBER, JsonDecoder_PeakNumber(thiz, token));
            thiz->index++;
            break;
    }

    return value;
}

TINY_LOR
static JsonArray * JsonDecoder_PeakArray(JsonDecoder *thiz)
{
    JsonArray * array = NULL;
    
    do
    {
        TokenAnalystResult result = TOKEN_ANALYSIS_OK;

        JsonToken *token = TinyList_GetAt(&thiz->tokens, thiz->index++);
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
            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL) 
            {
                result = TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_ARRAY_END)
            {
                thiz->index++;
                break;
            }

            // value
            value = JsonDecoder_PeakValue(thiz);
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
            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL) 
            {
                result = TOKEN_ANALYSIS_E_ARRAY_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_COMMA)
            {
                thiz->index++;
                continue;
            }

            if (token->type == JSON_TOKEN_ARRAY_END)
            {
                thiz->index++;
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
static JsonString * JsonDecoder_PeakString(JsonDecoder *thiz, JsonToken *token)
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
        memcpy(string->value, thiz->string + token->offset + 1, token->length - 2);
        string->length = token->length;
    } while (false);

    return string;
}

TINY_LOR
static JsonNumber * JsonDecoder_PeakNumber(JsonDecoder *thiz, JsonToken *token)
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
        memcpy(value, thiz->string + token->offset, token->length);

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
static JsonObject * JsonDecoder_PeakObject(JsonDecoder *thiz)
{
    JsonObject *object = NULL;

    do
    {
        TokenAnalystResult result = TOKEN_ANALYSIS_OK;

        JsonToken *token = TinyList_GetAt(&thiz->tokens, thiz->index++);
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
            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_OBJECT_END) 
            {
                thiz->index++;
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
            memcpy(key, thiz->string + token->offset + 1, token->length - 2);

            thiz->index++;

            // :
            token = TinyList_GetAt(&thiz->tokens, thiz->index);
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

            thiz->index++;

            // value
            value = JsonDecoder_PeakValue(thiz);
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
            token = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (token == NULL)
            {
                result = TOKEN_ANALYSIS_E_OBJECT_END_NOT_FOUND;
                break;
            }

            if (token->type == JSON_TOKEN_COMMA)
            {
                thiz->index++;
                continue;
            }

            if (token->type == JSON_TOKEN_OBJECT_END)
            {
                thiz->index++;
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
JsonObject * JsonDecoder_ConvertToObject(JsonDecoder *thiz)
{
    JsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    LOG_D(TAG, "JsonDecoder_ConvertToObject");

    //JsonDecoder_Print(thiz);

    thiz->index = 0;
    object = JsonDecoder_PeakObject(thiz);
    if (object == NULL)
    {
        JsonToken *token = TinyList_GetAt(&thiz->tokens, thiz->index);
        if (token != NULL) 
        {
            LOG_D(TAG, "INVALID JSON: error position is %d", token->offset);
        }
    }
    else
    {
        if (thiz->tokens.size > thiz->index)
        {
            JsonObject_Delete(object);
            object = NULL;
        }
    }

    return object;
}