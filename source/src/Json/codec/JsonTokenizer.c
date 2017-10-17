/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonTokenizer.h
 *
 * @remark
 *
 */

#include <tiny_log.h>
#include <ctype.h>
#include <tiny_char_util.h>
#include "JsonTokenizer.h"

#define TAG     "JsonTokenizer"

TINY_LOR
static void _OnTokenDelete (void * data, void *ctx)
{
    JsonToken_Delete((JsonToken *)data);
}

TINY_LOR
TinyRet JsonTokenizer_Construct(JsonTokenizer * thiz)
{
    TinyRet ret = TINY_RET_OK;

    ret = TinyQueue_Construct(&thiz->queue);
    if (RET_SUCCEEDED(ret))
    {
        TinyQueue_SetDeleteListener(&thiz->queue, _OnTokenDelete, thiz);
    }

    return ret;
}

TINY_LOR
void JsonTokenizer_Dispose(JsonTokenizer * thiz)
{
    TinyList_Dispose(&thiz->queue);
}

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

        if (RET_FAILED(TinyQueue_Push(&thiz->queue, token)))
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

        if (RET_FAILED(TinyQueue_Push(&thiz->queue, token)))
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

        if (RET_FAILED(TinyQueue_Push(&thiz->queue, token)))
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

        if (RET_FAILED(TinyQueue_Push(&thiz->queue, token)))
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
                        thiz->current ++;

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

        if (RET_FAILED(TinyQueue_Push(&thiz->queue, token)))
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
                thiz->current ++;
                c = *thiz->current;
            }

            if (c == '0')
            {
                thiz->current ++;
                c = *thiz->current;
            }
            else if (c >= '1' && c <= '9')
            {
                thiz->current ++;
                c = *thiz->current;

                while (isdigit(c))
                {
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
                thiz->current ++;
                c = *thiz->current;
            }

            if (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '}' || c == ']')
            {
                break;
            }
            else if (c == 'e' || c == 'E')
            {
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

        if (RET_FAILED(TinyQueue_Push(&thiz->queue, token)))
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
            thiz->current ++;
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
TinyRet JsonTokenizer_Parse(JsonTokenizer * thiz, const char *string, bool parseOnce)
{
    TokenParseResult result = TOKEN_PARSE_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(string, TINY_RET_E_ARG_NULL);

    thiz->string = string;
    thiz->current = string;
    thiz->parseOnce = parseOnce;

    if (parseOnce)
    {
        while (*thiz->current != '\0')
        {
            result = JsonTokenizer_ParseToken(thiz);
            if (result != TOKEN_PARSE_OK)
            {
                LOG_D(TAG, "JsonDecoder_ParseToken FAILED: %d", result);
                break;
            }
        }
    }

    return (result == TOKEN_PARSE_OK) ? TINY_RET_OK : TINY_RET_E_ARG_INVALID;
}

TINY_LOR
JsonToken * JsonTokenizer_Head(JsonTokenizer * thiz)
{
    JsonToken * token = NULL;

    if (thiz->parseOnce)
    {
        token = (JsonToken *) TinyQueue_Head(&thiz->queue);
    }
    else
    {
        while (true) 
        {
            if (JsonTokenizer_ParseToken(thiz) != TOKEN_PARSE_OK) 
            {
                break;
            }

            if (thiz->queue.size > 0) 
            {
                break;
            }
        }

        token = (JsonToken *) TinyQueue_Head(&thiz->queue);
    }

    return token;
}

TINY_LOR
void JsonTokenizer_Pop(JsonTokenizer * thiz)
{
    TinyQueue_Pop(&thiz->queue);
}