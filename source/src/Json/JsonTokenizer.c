/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonTokenizer.c
 *
 * @remark
 *
 */

#include <ctype.h>
#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_char_util.h>
#include "JsonTokenizer.h"
#include "JsonToken.h"
#include "JsonNumber.h"
#include "JsonString.h"
#include "JsonArray.h"

#define TAG     "JsonTokenizer"

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
    TOKEN_ANALYSIS_E_OBJECT_COLON_NOT_FOUND          = 4,
    TOKEN_ANALYSIS_E_OBJECT_VALUE_NOT_FOUND          = 5,
    TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID            = 6,
    TOKEN_ANALYSIS_E_OBJECT_END_UNNECESSARY_TOKEN    = 10,
} TokenAnalystResult;

TINY_LOR
static void _OnTokenDelete (void * data, void *ctx)
{
    tiny_free(data);
}

TINY_LOR
TinyRet JsonTokenizer_Construct(JsonTokenizer *thiz)
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
void JsonTokenizer_Dispose(JsonTokenizer *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyList_Dispose(&thiz->tokens);
}

TINY_LOR
static TokenParseResult JsonTokenizer_ParseTokenMonocase(JsonTokenizer *thiz, JsonTokenType type)
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

        thiz->current ++;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonTokenizer_ParseNull(JsonTokenizer *thiz)
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

        thiz->current += 4;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonTokenizer_ParseTrue(JsonTokenizer *thiz)
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

        thiz->current += 4;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonTokenizer_ParseFalse(JsonTokenizer *thiz)
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

        thiz->current += 5;
    } while (false);

    return result;
}

TINY_LOR
static TokenParseResult JsonTokenizer_ParseString(JsonTokenizer *thiz)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    do
    {
        uint32_t offset = (uint32_t) (thiz->current - thiz->string);
        const char *start = thiz->current;

        thiz->current ++;

        while (true)
        {
            char c = *thiz->current;

            if (c == '\\')
            {
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
                        thiz->current ++;
                        break;

                    case 'u':
                        thiz->current++;
                        if (is_hex(thiz->current[0]) && is_hex(thiz->current[1]) && is_hex(thiz->current[2]) && is_hex(thiz->current[3]))
                        {
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
                thiz->current ++;
                break;
            }

            else if (isprint(c))
            {
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
static TokenParseResult JsonTokenizer_ParseNumber(JsonTokenizer *thiz)
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
                thiz->current++;
                c = *thiz->current;
            }

            if (c == '0')
            {
                thiz->current++;
                c = *thiz->current;
            }
            else if (c >= '1' && c <= '9')
            {
                thiz->current++;
                c = *thiz->current;

                while (isdigit(c))
                {
                    thiz->current++;
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
                thiz->current++;
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
                thiz->current++;
                c = *thiz->current;
            }

            if (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '}' || c == ']')
            {
                break;
            }
            else if (c == 'e' || c == 'E')
            {
                thiz->current++;
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
static TokenParseResult JsonTokenizer_ParseToken(JsonTokenizer *thiz)
{
    switch (*thiz->current)
    {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            thiz->current++;
            return TOKEN_PARSE_OK;

        case '{':
            return JsonTokenizer_ParseTokenMonocase(thiz, JSON_TOKEN_OBJECT_START);

        case '}':
            return JsonTokenizer_ParseTokenMonocase(thiz, JSON_TOKEN_OBJECT_END);

        case '[':
            return JsonTokenizer_ParseTokenMonocase(thiz, JSON_TOKEN_ARRAY_START);

        case ']':
            return JsonTokenizer_ParseTokenMonocase(thiz, JSON_TOKEN_ARRAY_END);

        case ',':
            return JsonTokenizer_ParseTokenMonocase(thiz, JSON_TOKEN_COMMA);

        case ':':
            return JsonTokenizer_ParseTokenMonocase(thiz, JSON_TOKEN_COLON);

        case 'n':
            return JsonTokenizer_ParseNull(thiz);

        case 't':
            return JsonTokenizer_ParseTrue(thiz);

        case 'f':
            return JsonTokenizer_ParseFalse(thiz);

        case '"':
            return JsonTokenizer_ParseString(thiz);

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
            return JsonTokenizer_ParseNumber(thiz);

        default:
            return TOKEN_PARSE_E_INVALID_VALUE;
    }
}

TINY_LOR
TinyRet JsonTokenizer_Parse(JsonTokenizer *thiz, const char *string)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(string, TINY_RET_E_ARG_NULL);

    thiz->string = string;
    thiz->current = string;

    while (*thiz->current != '\0')
    {
        result = JsonTokenizer_ParseToken(thiz);
        if (result != TOKEN_PARSE_OK)
        {
            printf("JsonTokenizer_ParseToken: %d\n", result);
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

void JsonTokenizer_Print(JsonTokenizer *thiz)
{
    for (uint32_t i = 0; i < thiz->tokens.size; ++i)
    {
        JsonToken * token = TinyList_GetAt(&thiz->tokens, i);
        char buf[128];

        memset(buf, 0, 128);
        memcpy(buf, thiz->string + token->offset, token->length);
        printf("token: %s (%d, %d) -> %s\n", JsonToken_TypeToString(token->type), token->offset, token->length, buf);
    }

    printf("\n");
}
#endif


TINY_LOR
static JsonArray * JsonTokenizer_ToArray(JsonTokenizer *thiz)
{
    JsonArray * array = NULL;

    do
    {
        array = JsonArray_New();
        if (array == NULL)
        {
            break;
        }

        // TODO

    } while (false);

    return array;
}

TINY_LOR
static JsonString * JsonTokenizer_ToString(JsonTokenizer *thiz, JsonToken *token)
{
    JsonString * string = NULL;

    do
    {
        string = JsonString_New(NULL);
        if (string == NULL)
        {
            break;
        }

        string->value = tiny_malloc(token->length + 1);
        if (string->value == NULL)
        {
            JsonString_Delete(string);
            string = NULL;
            break;
        }

        memset(string->value, 0, token->length + 1);
        memcpy(string->value, thiz->string + token->offset, token->length);
        string->length = token->length;
    } while (false);

    return string;
}

TINY_LOR
static JsonNumber * JsonTokenizer_ToNumber(JsonTokenizer *thiz, JsonToken *token)
{
    JsonNumber * number = NULL;

    do
    {
        char value[128];

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
    } while (false);

    return number;
}

TINY_LOR
static JsonObject * JsonTokenizer_ToObject(JsonTokenizer *thiz)
{
    TokenAnalystResult result = TOKEN_ANALYSIS_OK;
    JsonObject *object = NULL;

    do
    {
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

        while ((thiz->tokens.size - thiz->index) >= 3)
        {
            char k[128];
            JsonToken *key = TinyList_GetAt(&thiz->tokens, thiz->index++);
            JsonToken *colon = TinyList_GetAt(&thiz->tokens, thiz->index++);
            JsonToken *value = TinyList_GetAt(&thiz->tokens, thiz->index);

            if (key == NULL)
            {
                LOG_D(TAG, "key not found");
                result = TOKEN_ANALYSIS_E_OBJECT_KEY_NOT_FOUND;
                break;
            }

            if (key->type != JSON_TOKEN_STRING)
            {
                LOG_D(TAG, "key type invalid: %d", key->type);
                result = TOKEN_ANALYSIS_E_OBJECT_KEY_NOT_FOUND;
                break;
            }

            if (colon == NULL)
            {
                LOG_D(TAG, "':' not found");
                result = TOKEN_ANALYSIS_E_OBJECT_COLON_NOT_FOUND;
                break;
            }

            if (colon->type != JSON_TOKEN_COLON)
            {
                LOG_D(TAG, "':' type invalid: %d", colon->type);
                result = TOKEN_ANALYSIS_E_OBJECT_COLON_NOT_FOUND;
                break;
            }

            if (value == NULL)
            {
                LOG_D(TAG, "value not found");
                result = TOKEN_ANALYSIS_E_OBJECT_VALUE_NOT_FOUND;
                break;
            }

            memset(k, 0, 128);
            memcpy(k, thiz->string + key->offset + 1, key->length - 1);

            switch (value->type)
            {
                case JSON_TOKEN_OBJECT_START:
                {
                    JsonObject *v = JsonTokenizer_ToObject(thiz);
                    if (v != NULL)
                    {
                        JsonObject_PutObject(object, k, v);
                    }
                    else
                    {
                        result = TOKEN_ANALYSIS_E_OBJECT_VALUE_NOT_FOUND;
                    }
                    break;
                }

                case JSON_TOKEN_OBJECT_END:
                    result = TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID;
                    break;

                case JSON_TOKEN_ARRAY_START:
                {
                    JsonArray *v = JsonTokenizer_ToArray(thiz);
                    if (v != NULL)
                    {
                        JsonObject_PutArray(object, k, v);
                    }
                    else
                    {
                        result = TOKEN_ANALYSIS_E_OBJECT_VALUE_NOT_FOUND;
                    }
                    break;
                }

                case JSON_TOKEN_ARRAY_END:
                    result = TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID;
                    break;

                case JSON_TOKEN_COMMA:
                    result = TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID;
                    break;

                case JSON_TOKEN_COLON:
                    result = TOKEN_ANALYSIS_E_OBJECT_VALUE_INVALID;
                    break;

                case JSON_TOKEN_NULL:
                {
                    JsonObject_PutNull(object, k);
                    break;
                }

                case JSON_TOKEN_TRUE:
                {
                    JsonObject_PutBoolean(object, k, true);
                    break;
                }

                case JSON_TOKEN_FALSE:
                {
                    JsonObject_PutBoolean(object, k, false);
                    break;
                }

                case JSON_TOKEN_STRING:
                {
                    JsonString *v = JsonTokenizer_ToString(thiz, value);
                    if (v != NULL)
                    {
                        JsonObject_PutString(object, k, v->value);
                    }
                    else
                    {
                        result = TOKEN_ANALYSIS_E_OBJECT_VALUE_NOT_FOUND;
                    }
                    break;
                }

                case JSON_TOKEN_NUMBER:
                {
                    JsonNumber *v = JsonTokenizer_ToNumber(thiz, value);
                    if (v != NULL)
                    {
                        JsonObject_PutNumber(object, k, v);
                    }
                    else
                    {
                        result = TOKEN_ANALYSIS_E_OBJECT_VALUE_NOT_FOUND;
                    }
                    break;
                }
            }

            if (result != TOKEN_ANALYSIS_OK)
            {
                break;
            }

            thiz->index++;

            JsonToken *comma = TinyList_GetAt(&thiz->tokens, thiz->index);
            if (comma->type != JSON_TOKEN_COMMA)
            {
                break;
            }

            thiz->index++;
        }

        if (result != TOKEN_ANALYSIS_OK)
        {
            JsonObject_Delete(object);
            break;
        }

        token = TinyList_GetAt(&thiz->tokens, thiz->index++);
        if (token == NULL)
        {
            JsonObject_Delete(object);
            break;
        }

        if (token->type != JSON_TOKEN_OBJECT_END)
        {
            JsonObject_Delete(object);
            break;
        }

        if (thiz->tokens.size > thiz->index)
        {
            JsonObject_Delete(object);
            break;
        }
    } while (0);

    return object;
}

TINY_LOR
JsonObject * JsonTokenizer_ConvertToObject(JsonTokenizer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    JsonTokenizer_Print(thiz);

    thiz->index = 0;

    return JsonTokenizer_ToObject(thiz);
}