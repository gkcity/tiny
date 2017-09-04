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

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_char_util.h>
#include "JsonTokenizer.h"
#include "JsonToken.h"

#define TAG     "JsonTokenizer"

typedef enum _TokenParseResult
{
    TOKEN_PARSE_OK = 0,
    TOKEN_PARSE_OK_BUT_OUT_OF_MEMORY = 1,
    TOKEN_PARSE_OK_BUT_ADD_LIST_FAILED = 2,
    TOKEN_PARSE_E_INVALID_VALUE = 3,
    TOKEN_PARSE_E_EXPECT_VALUE = 4,
    TOKEN_PARSE_E_INVALID_STRING = 5,
} TokenParseResult;

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
        const char *p = thiz->current;

        thiz->current ++;

        while (true)
        {
            char c = *thiz->current;

            if (c == '\\')
            {
                thiz->current ++;
                c = *thiz->current;
                if (c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't')
                {
                    thiz->current ++;
                }
                else if (c == 'u')
                {
                    thiz->current++;
                    if (is_hex(thiz->current[0]) && is_hex(thiz->current[1]) && is_hex(thiz->current[2]) && is_hex(thiz->current[3]))
                    {
                        thiz->current += 4;
                    }
                    else
                    {
                        result = TOKEN_PARSE_E_INVALID_STRING;
                    }
                }
                else
                {
                    result = TOKEN_PARSE_E_INVALID_STRING;
                }
            }

            else if (c == '"')
            {
                break;
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

        JsonToken * token = JsonToken_New(JSON_TOKEN_STRING, offset, (uint32_t)(thiz->current - p));
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
}
#endif