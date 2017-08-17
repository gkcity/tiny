/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   json.c
 *
 * @remark
 *
 */

#include "json.h"
#include <ctype.h>
#include "../TinyPortable/tiny_malloc.h"

static float _fabs(float x)
{
    return (x > 0) ? x : -x;
}

/* Apparently sscanf is not implemented in some "standard" libraries, so don't use it, if you don't have to. */
#define sscanf THINK_TWICE_ABOUT_USING_SSCANF

#define STARTING_CAPACITY               15
#define ARRAY_MAX_CAPACITY              122880 /* 15*(2^13) */
#define OBJECT_MAX_CAPACITY             960 /* 15*(2^6)  */
#define MAX_NESTING                     2048
#define DOUBLE_SERIALIZATION_FORMAT     "%f"

#define SIZEOF_TOKEN(a)                 (sizeof(a) - 1)
#define SKIP_CHAR(str)                  ((*str)++)
#define SKIP_WHITESPACES(str)           while (isspace(**str)) { SKIP_CHAR(str); }
#define MAX(a, b)                       ((a) > (b) ? (a) : (b))
#define IS_CONT(b)                      (((unsigned char)(b) & 0xC0) == 0x80) /* is utf-8 continuation byte */

/* Type definitions */
typedef union _JsonValue
{
    char *string;
    double number;
    JsonObject *object;
    JsonArray *array;
    int boolean;
    int null;
} JsonValue;

struct _JsonElement
{
    JsonElement *parent;
    JsonElementType type;
    JsonValue value;
};

struct _JsonObject
{
    JsonElement *wrapping_value;
    char **names;
    JsonElement **values;
    size_t count;
    size_t capacity;
};

struct _JsonArray
{
    JsonElement *wrapping_value;
    JsonElement **items;
    size_t count;
    size_t capacity;
};

/* Various */
//static void remove_comments(char *string, const char *start_token, const char *end_token);

TINY_LOR
static char *parson_strndup(const char *string, size_t n);

TINY_LOR
static char *parson_strdup(const char *string);

TINY_LOR
static int hex_char_to_int(char c);

TINY_LOR
static int parse_utf16_hex(const char *string, unsigned int *result);

TINY_LOR
static int num_bytes_in_utf8_sequence(unsigned char c);

TINY_LOR
static int verify_utf8_sequence(const unsigned char *string, int *len);

TINY_LOR
static int is_valid_utf8(const char *string, size_t string_len);

TINY_LOR
static int is_decimal(const char *string, size_t length);

/* JSON Object */
TINY_LOR
static JsonObject *json_object_init(JsonElement *wrapping_value);

TINY_LOR
static JsonResult json_object_add(JsonObject *object, const char *name, JsonElement *value);

TINY_LOR
static JsonResult json_object_resize(JsonObject *object, size_t new_capacity);

TINY_LOR
static JsonElement *json_object_nget_value(const JsonObject *object, const char *name, size_t n);

TINY_LOR
static void json_object_free(JsonObject *object);

/* JSON Array */
TINY_LOR
static JsonArray *json_array_init(JsonElement *wrapping_value);

TINY_LOR
static JsonResult json_array_add(JsonArray *array, JsonElement *value);

TINY_LOR
static JsonResult json_array_resize(JsonArray *array, size_t new_capacity);

TINY_LOR
static void json_array_free(JsonArray *array);

/* JSON Value */
TINY_LOR
static JsonElement *json_value_init_string_no_copy(char *string);

/* Parser */
TINY_LOR
static JsonResult skip_quotes(const char **string);

TINY_LOR
static int parse_utf16(const char **unprocessed, char **processed);

TINY_LOR
static char *process_string(const char *input, size_t len);

TINY_LOR
static char *get_quoted_string(const char **string);

TINY_LOR
static JsonElement *parse_object_value(const char **string, size_t nesting);

TINY_LOR
static JsonElement *parse_array_value(const char **string, size_t nesting);

TINY_LOR
static JsonElement *parse_string_value(const char **string);

TINY_LOR
static JsonElement *parse_boolean_value(const char **string);

TINY_LOR
static JsonElement *parse_number_value(const char **string);

TINY_LOR
static JsonElement *parse_null_value(const char **string);

TINY_LOR
static JsonElement *parse_value(const char **string, size_t nesting);

/* Serialization */
#if 1
TINY_LOR
static int json_serialize_to_buffer_r(const JsonElement *value, char *buf, int level, int is_pretty, char *num_buf);
#endif

#if 1
TINY_LOR
static int json_serialize_string(const char *string, char *buf);
#endif

#if 1
TINY_LOR
static int append_indent(char *buf, int level);
#endif

#if 1
TINY_LOR
static int append_string(char *buf, const char *string);
#endif

/* Various */
TINY_LOR
static char *parson_strndup(const char *string, size_t n)
{
    char *output_string = (char *) tiny_malloc(n + 1);
    if (!output_string)
    {
        return NULL;
    }
    output_string[n] = '\0';
    strncpy(output_string, string, n);
    return output_string;
}

TINY_LOR
static char *parson_strdup(const char *string)
{
    return parson_strndup(string, strlen(string));
}

TINY_LOR
static int hex_char_to_int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return -1;
}

TINY_LOR
static int parse_utf16_hex(const char *s, unsigned int *result)
{
    int x1, x2, x3, x4;
    if (s[0] == '\0' || s[1] == '\0' || s[2] == '\0' || s[3] == '\0')
    {
        return 0;
    }
    x1 = hex_char_to_int(s[0]);
    x2 = hex_char_to_int(s[1]);
    x3 = hex_char_to_int(s[2]);
    x4 = hex_char_to_int(s[3]);
    if (x1 == -1 || x2 == -1 || x3 == -1 || x4 == -1)
    {
        return 0;
    }
    *result = (unsigned int) ((x1 << 12) | (x2 << 8) | (x3 << 4) | x4);
    return 1;
}

TINY_LOR
static int num_bytes_in_utf8_sequence(unsigned char c)
{
    if (c == 0xC0 || c == 0xC1 || c > 0xF4 || IS_CONT(c))
    {
        return 0;
    }
    else if ((c & 0x80) == 0)
    {    /* 0xxxxxxx */
        return 1;
    }
    else if ((c & 0xE0) == 0xC0)
    { /* 110xxxxx */
        return 2;
    }
    else if ((c & 0xF0) == 0xE0)
    { /* 1110xxxx */
        return 3;
    }
    else if ((c & 0xF8) == 0xF0)
    { /* 11110xxx */
        return 4;
    }
    return 0; /* won't happen */
}

TINY_LOR
static int verify_utf8_sequence(const unsigned char *string, int *len)
{
    unsigned int cp = 0;
    *len = num_bytes_in_utf8_sequence(string[0]);

    if (*len == 1)
    {
        cp = string[0];
    }
    else if (*len == 2 && IS_CONT(string[1]))
    {
        cp = string[0] & 0x1F;
        cp = (cp << 6) | (string[1] & 0x3F);
    }
    else if (*len == 3 && IS_CONT(string[1]) && IS_CONT(string[2]))
    {
        cp = ((unsigned char) string[0]) & 0xF;
        cp = (cp << 6) | (string[1] & 0x3F);
        cp = (cp << 6) | (string[2] & 0x3F);
    }
    else if (*len == 4 && IS_CONT(string[1]) && IS_CONT(string[2]) && IS_CONT(string[3]))
    {
        cp = string[0] & 0x7;
        cp = (cp << 6) | (string[1] & 0x3F);
        cp = (cp << 6) | (string[2] & 0x3F);
        cp = (cp << 6) | (string[3] & 0x3F);
    }
    else
    {
        return 0;
    }

    /* overlong encodings */
    if ((cp < 0x80 && *len > 1) ||
        (cp < 0x800 && *len > 2) ||
        (cp < 0x10000 && *len > 3))
    {
        return 0;
    }

    /* invalid unicode */
    if (cp > 0x10FFFF)
    {
        return 0;
    }

    /* surrogate halves */
    if (cp >= 0xD800 && cp <= 0xDFFF)
    {
        return 0;
    }

    return 1;
}

TINY_LOR
static int is_valid_utf8(const char *string, size_t string_len)
{
    int len = 0;
    const char *string_end = string + string_len;
    while (string < string_end)
    {
        if (!verify_utf8_sequence((const unsigned char *) string, &len))
        {
            return 0;
        }
        string += len;
    }
    return 1;
}

TINY_LOR
static int is_decimal(const char *string, size_t length)
{
    if (length > 1 && string[0] == '0' && string[1] != '.')
    {
        return 0;
    }
    if (length > 2 && !strncmp(string, "-0", 2) && string[2] != '.')
    {
        return 0;
    }
    while (length--)
    {
        if (strchr("xX", string[length]))
        {
            return 0;
        }
    }
    return 1;
}

#if 0
static void remove_comments(char *string, const char *start_token, const char *end_token)
{
    int in_string = 0, escaped = 0;
    size_t i;
    char *ptr = NULL, current_char;
    size_t start_token_len = strlen(start_token);
    size_t end_token_len = strlen(end_token);
    if (start_token_len == 0 || end_token_len == 0)
    {
        return;
    }
    while ((current_char = *string) != '\0')
    {
        if (current_char == '\\' && !escaped)
        {
            escaped = 1;
            string++;
            continue;
        }
        else if (current_char == '\"' && !escaped)
        {
            in_string = !in_string;
        }
        else if (!in_string && strncmp(string, start_token, start_token_len) == 0)
        {
            for (i = 0; i < start_token_len; i++)
            {
                string[i] = ' ';
            }
            string = string + start_token_len;
            ptr = strstr(string, end_token);
            if (!ptr)
            {
                return;
            }
            for (i = 0; i < (ptr - string) + end_token_len; i++)
            {
                string[i] = ' ';
            }
            string = ptr + end_token_len - 1;
        }
        escaped = 0;
        string++;
    }
}
#endif

/* JSON Object */
TINY_LOR
static JsonObject *json_object_init(JsonElement *wrapping_value)
{
    JsonObject *new_obj = (JsonObject *) tiny_malloc(sizeof(JsonObject));
    if (new_obj == NULL)
    {
        return NULL;
    }
    new_obj->wrapping_value = wrapping_value;
    new_obj->names = (char **) NULL;
    new_obj->values = (JsonElement **) NULL;
    new_obj->capacity = 0;
    new_obj->count = 0;
    return new_obj;
}

TINY_LOR
static JsonResult json_object_add(JsonObject *object, const char *name, JsonElement *value)
{
    size_t index = 0;
    if (object == NULL || name == NULL || value == NULL)
    {
        return JSON_FAILURE;
    }
    if (json_object_get_value(object, name) != NULL)
    {
        return JSON_FAILURE;
    }
    if (object->count >= object->capacity)
    {
        size_t new_capacity = MAX(object->capacity * 2, STARTING_CAPACITY);
        if (new_capacity > OBJECT_MAX_CAPACITY)
        {
            return JSON_FAILURE;
        }
        if (json_object_resize(object, new_capacity) == JSON_FAILURE)
        {
            return JSON_FAILURE;
        }
    }
    index = object->count;
    object->names[index] = parson_strdup(name);
    if (object->names[index] == NULL)
    {
        return JSON_FAILURE;
    }
    value->parent = json_object_get_wrapping_value(object);
    object->values[index] = value;
    object->count++;
    return JSON_SUCCESS;
}

TINY_LOR
static JsonResult json_object_resize(JsonObject *object, size_t new_capacity)
{
    char **temp_names = NULL;
    JsonElement **temp_values = NULL;

    if ((object->names == NULL && object->values != NULL) ||
        (object->names != NULL && object->values == NULL) ||
        new_capacity == 0)
    {
        return JSON_FAILURE; /* Shouldn't happen */
    }
    temp_names = (char **) tiny_malloc(new_capacity * sizeof(char *));
    if (temp_names == NULL)
    {
        return JSON_FAILURE;
    }
    temp_values = (JsonElement **) tiny_malloc(new_capacity * sizeof(JsonElement *));
    if (temp_values == NULL)
    {
        tiny_free(temp_names);
        return JSON_FAILURE;
    }
    if (object->names != NULL && object->values != NULL && object->count > 0)
    {
        memcpy(temp_names, object->names, object->count * sizeof(char *));
        memcpy(temp_values, object->values, object->count * sizeof(JsonElement *));
    }
    tiny_free(object->names);
    tiny_free(object->values);
    object->names = temp_names;
    object->values = temp_values;
    object->capacity = new_capacity;
    return JSON_SUCCESS;
}

TINY_LOR
static JsonElement *json_object_nget_value(const JsonObject *object, const char *name, size_t n)
{
    size_t i, name_length;
    for (i = 0; i < json_object_get_count(object); i++)
    {
        name_length = strlen(object->names[i]);
        if (name_length != n)
        {
            continue;
        }
        if (strncmp(object->names[i], name, n) == 0)
        {
            return object->values[i];
        }
    }
    return NULL;
}

TINY_LOR
static void json_object_free(JsonObject *object)
{
    size_t i;
    for (i = 0; i < object->count; i++)
    {
        tiny_free(object->names[i]);
        json_value_free(object->values[i]);
    }
    tiny_free(object->names);
    tiny_free(object->values);
    tiny_free(object);
}

/* JSON Array */
TINY_LOR
static JsonArray *json_array_init(JsonElement *wrapping_value)
{
    JsonArray *new_array = (JsonArray *) tiny_malloc(sizeof(JsonArray));
    if (new_array == NULL)
    {
        return NULL;
    }
    new_array->wrapping_value = wrapping_value;
    new_array->items = (JsonElement **) NULL;
    new_array->capacity = 0;
    new_array->count = 0;
    return new_array;
}

TINY_LOR
static JsonResult json_array_add(JsonArray *array, JsonElement *value)
{
    if (array->count >= array->capacity)
    {
        size_t new_capacity = MAX(array->capacity * 2, STARTING_CAPACITY);
        if (new_capacity > ARRAY_MAX_CAPACITY)
        {
            return JSON_FAILURE;
        }
        if (json_array_resize(array, new_capacity) == JSON_FAILURE)
        {
            return JSON_FAILURE;
        }
    }
    value->parent = json_array_get_wrapping_value(array);
    array->items[array->count] = value;
    array->count++;
    return JSON_SUCCESS;
}

TINY_LOR
static JsonResult json_array_resize(JsonArray *array, size_t new_capacity)
{
    JsonElement **new_items = NULL;
    if (new_capacity == 0)
    {
        return JSON_FAILURE;
    }

    new_items = (JsonElement **) tiny_malloc(new_capacity * sizeof(JsonElement *));
    if (new_items == NULL)
    {
        return JSON_FAILURE;
    }

    if (array->items != NULL && array->count > 0)
    {
        memcpy(new_items, array->items, array->count * sizeof(JsonElement *));
    }

    tiny_free(array->items);
    array->items = new_items;
    array->capacity = new_capacity;
    return JSON_SUCCESS;
}

TINY_LOR
static void json_array_free(JsonArray *array)
{
    size_t i;
    for (i = 0; i < array->count; i++)
    {
        json_value_free(array->items[i]);
    }

    tiny_free(array->items);
    tiny_free(array);
}

/* JSON Value */
TINY_LOR
static JsonElement *json_value_init_string_no_copy(char *string)
{
    JsonElement *new_value = (JsonElement *) tiny_malloc(sizeof(JsonElement));
    if (!new_value)
    {
        return NULL;
    }

    new_value->parent = NULL;
    new_value->type = JSON_STRING;
    new_value->value.string = string;
    return new_value;
}

/* Parser */
TINY_LOR
static JsonResult skip_quotes(const char **string)
{
    if (**string != '\"')
    {
        return JSON_FAILURE;
    }

    SKIP_CHAR(string);
    while (**string != '\"')
    {
        if (**string == '\0')
        {
            return JSON_FAILURE;
        }
        else if (**string == '\\')
        {
            SKIP_CHAR(string);
            if (**string == '\0')
            {
                return JSON_FAILURE;
            }
        }
        SKIP_CHAR(string);
    }
    SKIP_CHAR(string);

    return JSON_SUCCESS;
}

TINY_LOR
static int parse_utf16(const char **unprocessed, char **processed)
{
    unsigned int cp, lead, trail;
    int parse_succeeded = 0;
    char *processed_ptr = *processed;
    const char *unprocessed_ptr = *unprocessed;
    unprocessed_ptr++; /* skips u */
    parse_succeeded = parse_utf16_hex(unprocessed_ptr, &cp);
    if (!parse_succeeded)
    {
        return JSON_FAILURE;
    }
    if (cp < 0x80)
    {
        *processed_ptr = (char) cp; /* 0xxxxxxx */
    }
    else if (cp < 0x800)
    {
        *processed_ptr++ = ((cp >> 6) & 0x1F) | 0xC0; /* 110xxxxx */
        *processed_ptr = ((cp) & 0x3F) | 0x80; /* 10xxxxxx */
    }
    else if (cp < 0xD800 || cp > 0xDFFF)
    {
        *processed_ptr++ = ((cp >> 12) & 0x0F) | 0xE0; /* 1110xxxx */
        *processed_ptr++ = ((cp >> 6) & 0x3F) | 0x80; /* 10xxxxxx */
        *processed_ptr = ((cp) & 0x3F) | 0x80; /* 10xxxxxx */
    }
    else if (cp >= 0xD800 && cp <= 0xDBFF)
    { /* lead surrogate (0xD800..0xDBFF) */
        lead = cp;
        unprocessed_ptr += 4; /* should always be within the buffer, otherwise previous sscanf would fail */
        if (*unprocessed_ptr++ != '\\' || *unprocessed_ptr++ != 'u')
        {
            return JSON_FAILURE;
        }
        parse_succeeded = parse_utf16_hex(unprocessed_ptr, &trail);
        if (!parse_succeeded || trail < 0xDC00 || trail > 0xDFFF)
        { /* valid trail surrogate? (0xDC00..0xDFFF) */
            return JSON_FAILURE;
        }
        cp = ((((lead - 0xD800) & 0x3FF) << 10) | ((trail - 0xDC00) & 0x3FF)) + 0x010000;
        *processed_ptr++ = (((cp >> 18) & 0x07) | 0xF0); /* 11110xxx */
        *processed_ptr++ = (((cp >> 12) & 0x3F) | 0x80); /* 10xxxxxx */
        *processed_ptr++ = (((cp >> 6) & 0x3F) | 0x80); /* 10xxxxxx */
        *processed_ptr = (((cp) & 0x3F) | 0x80); /* 10xxxxxx */
    }
    else
    { /* trail surrogate before lead surrogate */
        return JSON_FAILURE;
    }
    unprocessed_ptr += 3;
    *processed = processed_ptr;
    *unprocessed = unprocessed_ptr;
    return JSON_SUCCESS;
}


/* Copies and processes passed string up to supplied length.
Example: "\u006Corem ipsum" -> lorem ipsum */
TINY_LOR
static char *process_string(const char *input, size_t len)
{
    const char *input_ptr = input;
    size_t initial_size = (len + 1) * sizeof(char);
    size_t final_size = 0;
    char *output = NULL, *output_ptr = NULL, *resized_output = NULL;
    output = (char *) tiny_malloc(initial_size);
    if (output == NULL)
    {
        goto error;
    }
    output_ptr = output;
    while ((*input_ptr != '\0') && (size_t) (input_ptr - input) < len)
    {
        if (*input_ptr == '\\')
        {
            input_ptr++;
            switch (*input_ptr)
            {
                case '\"':
                    *output_ptr = '\"';
                    break;
                case '\\':
                    *output_ptr = '\\';
                    break;
                case '/':
                    *output_ptr = '/';
                    break;
                case 'b':
                    *output_ptr = '\b';
                    break;
                case 'f':
                    *output_ptr = '\f';
                    break;
                case 'n':
                    *output_ptr = '\n';
                    break;
                case 'r':
                    *output_ptr = '\r';
                    break;
                case 't':
                    *output_ptr = '\t';
                    break;
                case 'u':
                    if (parse_utf16(&input_ptr, &output_ptr) == JSON_FAILURE)
                    {
                        goto error;
                    }
                    break;
                default:
                    goto error;
            }
        }
        else if ((unsigned char) *input_ptr < 0x20)
        {
            goto error; /* 0x00-0x19 are invalid characters for TinyJson string (http://www.ietf.org/rfc/rfc4627.txt) */
        }
        else
        {
            *output_ptr = *input_ptr;
        }
        output_ptr++;
        input_ptr++;
    }
    *output_ptr = '\0';
    /* resize to new length */
    final_size = (size_t) (output_ptr - output) + 1;
    /* todo: don't resize if final_size == initial_size */
    resized_output = (char *) tiny_malloc(final_size);
    if (resized_output == NULL)
    {
        goto error;
    }
    memcpy(resized_output, output, final_size);
    tiny_free(output);
    return resized_output;
    error:
    tiny_free(output);
    return NULL;
}

/* Return processed contents of a string between quotes and
   skips passed argument to a matching quote. */
TINY_LOR
static char *get_quoted_string(const char **string)
{
    const char *string_start = *string;
    size_t string_len = 0;
    JsonResult status = skip_quotes(string);
    if (status != JSON_SUCCESS)
    {
        return NULL;
    }
    string_len = *string - string_start - 2; /* length without quotes */
    return process_string(string_start + 1, string_len);
}

TINY_LOR
static JsonElement *parse_value(const char **string, size_t nesting)
{
    if (nesting > MAX_NESTING)
    {
        return NULL;
    }
    SKIP_WHITESPACES(string);
    switch (**string)
    {
        case '{':
            return parse_object_value(string, nesting + 1);
        case '[':
            return parse_array_value(string, nesting + 1);
        case '\"':
            return parse_string_value(string);
        case 'f':
        case 't':
            return parse_boolean_value(string);
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
            return parse_number_value(string);
        case 'n':
            return parse_null_value(string);
        default:
            return NULL;
    }
}

TINY_LOR
static JsonElement *parse_object_value(const char **string, size_t nesting)
{
    JsonElement *output_value = json_value_init_object(), *new_value = NULL;
    JsonObject *output_object = json_value_get_object(output_value);
    char *new_key = NULL;
    if (output_value == NULL || **string != '{')
    {
        return NULL;
    }
    SKIP_CHAR(string);
    SKIP_WHITESPACES(string);
    if (**string == '}')
    { /* empty object */
        SKIP_CHAR(string);
        return output_value;
    }
    while (**string != '\0')
    {
        new_key = get_quoted_string(string);
        SKIP_WHITESPACES(string);
        if (new_key == NULL || **string != ':')
        {
            json_value_free(output_value);
            return NULL;
        }
        SKIP_CHAR(string);
        new_value = parse_value(string, nesting);
        if (new_value == NULL)
        {
            tiny_free(new_key);
            json_value_free(output_value);
            return NULL;
        }
        if (json_object_add(output_object, new_key, new_value) == JSON_FAILURE)
        {
            tiny_free(new_key);
            json_value_free(new_value);
            json_value_free(output_value);
            return NULL;
        }
        tiny_free(new_key);
        SKIP_WHITESPACES(string);
        if (**string != ',')
        {
            break;
        }
        SKIP_CHAR(string);
        SKIP_WHITESPACES(string);
    }
    SKIP_WHITESPACES(string);
    if (**string != '}' || /* Trim object after parsing is over */
        json_object_resize(output_object, json_object_get_count(output_object)) == JSON_FAILURE)
    {
        json_value_free(output_value);
        return NULL;
    }
    SKIP_CHAR(string);
    return output_value;
}

TINY_LOR
static JsonElement *parse_array_value(const char **string, size_t nesting)
{
    JsonElement *output_value = json_value_init_array(), *new_array_value = NULL;
    JsonArray *output_array = json_value_get_array(output_value);
    if (!output_value || **string != '[')
    {
        return NULL;
    }
    SKIP_CHAR(string);
    SKIP_WHITESPACES(string);
    if (**string == ']')
    { /* empty array */
        SKIP_CHAR(string);
        return output_value;
    }
    while (**string != '\0')
    {
        new_array_value = parse_value(string, nesting);
        if (new_array_value == NULL)
        {
            json_value_free(output_value);
            return NULL;
        }
        if (json_array_add(output_array, new_array_value) == JSON_FAILURE)
        {
            json_value_free(new_array_value);
            json_value_free(output_value);
            return NULL;
        }
        SKIP_WHITESPACES(string);
        if (**string != ',')
        {
            break;
        }
        SKIP_CHAR(string);
        SKIP_WHITESPACES(string);
    }
    SKIP_WHITESPACES(string);
    if (**string != ']' || /* Trim array after parsing is over */
        json_array_resize(output_array, json_array_get_count(output_array)) == JSON_FAILURE)
    {
        json_value_free(output_value);
        return NULL;
    }
    SKIP_CHAR(string);
    return output_value;
}

TINY_LOR
static JsonElement *parse_string_value(const char **string)
{
    JsonElement *value = NULL;
    char *new_string = get_quoted_string(string);
    if (new_string == NULL)
    {
        return NULL;
    }
    value = json_value_init_string_no_copy(new_string);
    if (value == NULL)
    {
        tiny_free(new_string);
        return NULL;
    }
    return value;
}

TINY_LOR
static JsonElement *parse_boolean_value(const char **string)
{
    size_t true_token_size = SIZEOF_TOKEN("true");
    size_t false_token_size = SIZEOF_TOKEN("false");
    if (strncmp("true", *string, true_token_size) == 0)
    {
        *string += true_token_size;
        return json_value_init_boolean(1);
    }
    else if (strncmp("false", *string, false_token_size) == 0)
    {
        *string += false_token_size;
        return json_value_init_boolean(0);
    }
    return NULL;
}

TINY_LOR
static JsonElement *parse_number_value(const char **string)
{
    char *end;
    double number = 0;
    //errno = 0;

    number = strtod(*string, &end);
    //if (errno || !is_decimal(*string, end - *string))
    if (!is_decimal(*string, end - *string))
    {
        return NULL;
    }
    *string = end;
    return json_value_init_number(number);
}

TINY_LOR
static JsonElement *parse_null_value(const char **string)
{
    size_t token_size = SIZEOF_TOKEN("null");
    if (strncmp("null", *string, token_size) == 0)
    {
        *string += token_size;
        return json_value_init_null();
    }
    return NULL;
}

/* Serialization */
#define APPEND_STRING(str) do { written = append_string(buf, (str));\
                                if (written < 0) { return -1; }\
                                if (buf != NULL) { buf += written; }\
                                written_total += written; } while(0)

#define APPEND_INDENT(level) do { written = append_indent(buf, (level));\
                                  if (written < 0) { return -1; }\
                                  if (buf != NULL) { buf += written; }\
                                  written_total += written; } while(0)

#if 1
TINY_LOR
static int json_serialize_to_buffer_r(const JsonElement *value, char *buf, int level, int is_pretty, char *num_buf)
{
    const char *key = NULL, *string = NULL;
    JsonElement *temp_value = NULL;
    JsonArray *array = NULL;
    JsonObject *object = NULL;
    size_t i = 0, count = 0;
    double num = 0.0;
    int written = -1, written_total = 0;

    switch (json_value_get_type(value))
    {
        case JSON_ARRAY:
            array = json_value_get_array(value);
            count = json_array_get_count(array);
            APPEND_STRING("[");
            if (count > 0 && is_pretty)
            {
                APPEND_STRING("\n");
            }
            for (i = 0; i < count; i++)
            {
                if (is_pretty)
                {
                    APPEND_INDENT(level + 1);
                }
                temp_value = json_array_get_value(array, i);
                written = json_serialize_to_buffer_r(temp_value, buf, level + 1, is_pretty, num_buf);
                if (written < 0)
                {
                    return -1;
                }
                if (buf != NULL)
                {
                    buf += written;
                }
                written_total += written;
                if (i < (count - 1))
                {
                    APPEND_STRING(",");
                }
                if (is_pretty)
                {
                    APPEND_STRING("\n");
                }
            }
            if (count > 0 && is_pretty)
            {
                APPEND_INDENT(level);
            }
            APPEND_STRING("]");
            return written_total;
        case JSON_OBJECT:
            object = json_value_get_object(value);
            count = json_object_get_count(object);
            APPEND_STRING("{");
            if (count > 0 && is_pretty)
            {
                APPEND_STRING("\n");
            }
            for (i = 0; i < count; i++)
            {
                key = json_object_get_name(object, i);
                if (key == NULL)
                {
                    return -1;
                }
                if (is_pretty)
                {
                    APPEND_INDENT(level + 1);
                }
                written = json_serialize_string(key, buf);
                if (written < 0)
                {
                    return -1;
                }
                if (buf != NULL)
                {
                    buf += written;
                }
                written_total += written;
                APPEND_STRING(":");
                if (is_pretty)
                {
                    APPEND_STRING(" ");
                }
                temp_value = json_object_get_value(object, key);
                written = json_serialize_to_buffer_r(temp_value, buf, level + 1, is_pretty, num_buf);
                if (written < 0)
                {
                    return -1;
                }
                if (buf != NULL)
                {
                    buf += written;
                }
                written_total += written;
                if (i < (count - 1))
                {
                    APPEND_STRING(",");
                }
                if (is_pretty)
                {
                    APPEND_STRING("\n");
                }
            }
            if (count > 0 && is_pretty)
            {
                APPEND_INDENT(level);
            }
            APPEND_STRING("}");
            return written_total;
        case JSON_STRING:
            string = json_value_get_string(value);
            if (string == NULL)
            {
                return -1;
            }
            written = json_serialize_string(string, buf);
            if (written < 0)
            {
                return -1;
            }
            if (buf != NULL)
            {
                buf += written;
            }
            written_total += written;
            return written_total;
        case JSON_BOOLEAN:
            if (json_value_get_boolean(value))
            {
                APPEND_STRING("true");
            }
            else
            {
                APPEND_STRING("false");
            }
            return written_total;
        case JSON_NUMBER:
            num = json_value_get_number(value);
            if (buf != NULL)
            {
                num_buf = buf;
            }
            if (num == ((double) (int) num))
            { /*  check if num is integer */
                written = sprintf(num_buf, "%d", (int) num);
            }
            else if (num == ((double) (unsigned int) num))
            {
                written = sprintf(num_buf, "%u", (unsigned int) num);
            }
            else
            {
                written = sprintf(num_buf, DOUBLE_SERIALIZATION_FORMAT, num);
            }
            if (written < 0)
            {
                return -1;
            }
            if (buf != NULL)
            {
                buf += written;
            }
            written_total += written;
            return written_total;
        case JSON_NULL:
            APPEND_STRING("null");
            return written_total;
        case JSON_ERROR:
            return -1;
        default:
            return -1;
    }
}
#endif

#if 1
TINY_LOR
static int json_serialize_string(const char *string, char *buf)
{
    size_t i = 0, len = strlen(string);
    char c = '\0';
    int written = -1, written_total = 0;
    APPEND_STRING("\"");
    for (i = 0; i < len; i++)
    {
        c = string[i];
        switch (c)
        {
            case '\"':
                APPEND_STRING("\\\"");
                break;
            case '\\':
                APPEND_STRING("\\\\");
                break;
            case '/':
                APPEND_STRING("\\/");
                break; /* to make TinyJson embeddable in xml\/html */
            case '\b':
                APPEND_STRING("\\b");
                break;
            case '\f':
                APPEND_STRING("\\f");
                break;
            case '\n':
                APPEND_STRING("\\n");
                break;
            case '\r':
                APPEND_STRING("\\r");
                break;
            case '\t':
                APPEND_STRING("\\t");
                break;
            case '\x00':
                APPEND_STRING("\\u0000");
                break;
            case '\x01':
                APPEND_STRING("\\u0001");
                break;
            case '\x02':
                APPEND_STRING("\\u0002");
                break;
            case '\x03':
                APPEND_STRING("\\u0003");
                break;
            case '\x04':
                APPEND_STRING("\\u0004");
                break;
            case '\x05':
                APPEND_STRING("\\u0005");
                break;
            case '\x06':
                APPEND_STRING("\\u0006");
                break;
            case '\x07':
                APPEND_STRING("\\u0007");
                break;
                /* '\x08' duplicate: '\b' */
                /* '\x09' duplicate: '\t' */
                /* '\x0a' duplicate: '\n' */
            case '\x0b':
                APPEND_STRING("\\u000b");
                break;
                /* '\x0c' duplicate: '\f' */
                /* '\x0d' duplicate: '\r' */
            case '\x0e':
                APPEND_STRING("\\u000e");
                break;
            case '\x0f':
                APPEND_STRING("\\u000f");
                break;
            case '\x10':
                APPEND_STRING("\\u0010");
                break;
            case '\x11':
                APPEND_STRING("\\u0011");
                break;
            case '\x12':
                APPEND_STRING("\\u0012");
                break;
            case '\x13':
                APPEND_STRING("\\u0013");
                break;
            case '\x14':
                APPEND_STRING("\\u0014");
                break;
            case '\x15':
                APPEND_STRING("\\u0015");
                break;
            case '\x16':
                APPEND_STRING("\\u0016");
                break;
            case '\x17':
                APPEND_STRING("\\u0017");
                break;
            case '\x18':
                APPEND_STRING("\\u0018");
                break;
            case '\x19':
                APPEND_STRING("\\u0019");
                break;
            case '\x1a':
                APPEND_STRING("\\u001a");
                break;
            case '\x1b':
                APPEND_STRING("\\u001b");
                break;
            case '\x1c':
                APPEND_STRING("\\u001c");
                break;
            case '\x1d':
                APPEND_STRING("\\u001d");
                break;
            case '\x1e':
                APPEND_STRING("\\u001e");
                break;
            case '\x1f':
                APPEND_STRING("\\u001f");
                break;
            default:
                if (buf != NULL)
                {
                    buf[0] = c;
                    buf += 1;
                }
                written_total += 1;
                break;
        }
    }
    APPEND_STRING("\"");
    return written_total;
}
#endif

#if 1
TINY_LOR
static int append_indent(char *buf, int level)
{
    int i;
    int written = -1, written_total = 0;
    for (i = 0; i < level; i++)
    {
        APPEND_STRING("    ");
    }
    return written_total;
}
#endif

#if 1
TINY_LOR
static int append_string(char *buf, const char *string)
{
    if (buf == NULL)
    {
        return (int) strlen(string);
    }
    return sprintf(buf, "%s", string);
}
#endif

#undef APPEND_STRING
#undef APPEND_INDENT

TINY_LOR
JsonElement *json_parse_string(const char *string)
{
    if (string == NULL)
    {
        return NULL;
    }
    if (string[0] == '\xEF' && string[1] == '\xBB' && string[2] == '\xBF')
    {
        string = string + 3; /* Support for UTF-8 BOM */
    }
    return parse_value((const char **) &string, 0);
}

/* JSON Object API */

TINY_LOR
JsonElement *json_object_get_value(const JsonObject *object, const char *name)
{
    if (object == NULL || name == NULL)
    {
        return NULL;
    }
    return json_object_nget_value(object, name, strlen(name));
}

TINY_LOR
const char *json_object_get_string(const JsonObject *object, const char *name)
{
    return json_value_get_string(json_object_get_value(object, name));
}

TINY_LOR
double json_object_get_number(const JsonObject *object, const char *name)
{
    return json_value_get_number(json_object_get_value(object, name));
}

TINY_LOR
JsonObject *json_object_get_object(const JsonObject *object, const char *name)
{
    return json_value_get_object(json_object_get_value(object, name));
}

TINY_LOR
JsonArray *json_object_get_array(const JsonObject *object, const char *name)
{
    return json_value_get_array(json_object_get_value(object, name));
}

TINY_LOR
int json_object_get_boolean(const JsonObject *object, const char *name)
{
    return json_value_get_boolean(json_object_get_value(object, name));
}

TINY_LOR
JsonElement *json_object_dotget_value(const JsonObject *object, const char *name)
{
    const char *dot_position = strchr(name, '.');
    if (!dot_position)
    {
        return json_object_get_value(object, name);
    }
    object = json_value_get_object(json_object_nget_value(object, name, dot_position - name));
    return json_object_dotget_value(object, dot_position + 1);
}

//TINY_LOR
//const char *json_object_dotget_string(const JsonObject *object, const char *name)
//{
//    return json_value_get_string(json_object_dotget_value(object, name));
//}
//
//TINY_LOR
//double json_object_dotget_number(const JsonObject *object, const char *name)
//{
//    return json_value_get_number(json_object_dotget_value(object, name));
//}
//
//TINY_LOR
//JsonObject *json_object_dotget_object(const JsonObject *object, const char *name)
//{
//    return json_value_get_object(json_object_dotget_value(object, name));
//}
//
//TINY_LOR
//JsonArray *json_object_dotget_array(const JsonObject *object, const char *name)
//{
//    return json_value_get_array(json_object_dotget_value(object, name));
//}
//
//TINY_LOR
//int json_object_dotget_boolean(const JsonObject *object, const char *name)
//{
//    return json_value_get_boolean(json_object_dotget_value(object, name));
//}

TINY_LOR
size_t json_object_get_count(const JsonObject *object)
{
    return object ? object->count : 0;
}

TINY_LOR
const char *json_object_get_name(const JsonObject *object, size_t index)
{
    if (object == NULL || index >= json_object_get_count(object))
    {
        return NULL;
    }
    return object->names[index];
}

//TINY_LOR
//JsonElement *json_object_get_value_at(const JsonObject *object, size_t index)
//{
//    if (object == NULL || index >= json_object_get_count(object))
//    {
//        return NULL;
//    }
//    return object->values[index];
//}

TINY_LOR
JsonElement *json_object_get_wrapping_value(const JsonObject *object)
{
    return object->wrapping_value;
}

//TINY_LOR
//int json_object_has_value(const JsonObject *object, const char *name)
//{
//    return json_object_get_value(object, name) != NULL;
//}

TINY_LOR
int json_object_has_value_of_type(const JsonObject *object, const char *name, JsonElementType type)
{
    JsonElement *val = json_object_get_value(object, name);
    return val != NULL && json_value_get_type(val) == type;
}

//TINY_LOR
//int json_object_dothas_value(const JsonObject *object, const char *name)
//{
//    return json_object_dotget_value(object, name) != NULL;
//}
//
//TINY_LOR
//int json_object_dothas_value_of_type(const JsonObject *object, const char *name, JsonElementType type)
//{
//    JsonElement *val = json_object_dotget_value(object, name);
//    return val != NULL && json_value_get_type(val) == type;
//}

/* JSON Array API */
TINY_LOR
JsonElement *json_array_get_value(const JsonArray *array, size_t index)
{
    if (array == NULL || index >= json_array_get_count(array))
    {
        return NULL;
    }
    return array->items[index];
}

//TINY_LOR
//const char *json_array_get_string(const JsonArray *array, size_t index)
//{
//    return json_value_get_string(json_array_get_value(array, index));
//}
//
//TINY_LOR
//double json_array_get_number(const JsonArray *array, size_t index)
//{
//    return json_value_get_number(json_array_get_value(array, index));
//}

TINY_LOR
JsonObject *json_array_get_object(const JsonArray *array, size_t index)
{
    return json_value_get_object(json_array_get_value(array, index));
}

//TINY_LOR
//JsonArray *json_array_get_array(const JsonArray *array, size_t index)
//{
//    return json_value_get_array(json_array_get_value(array, index));
//}
//
//TINY_LOR
//int json_array_get_boolean(const JsonArray *array, size_t index)
//{
//    return json_value_get_boolean(json_array_get_value(array, index));
//}

TINY_LOR
size_t json_array_get_count(const JsonArray *array)
{
    return array ? array->count : 0;
}

TINY_LOR
JsonElement *json_array_get_wrapping_value(const JsonArray *array)
{
    return array->wrapping_value;
}

/* JSON Value API */
TINY_LOR
JsonElementType json_value_get_type(const JsonElement *value)
{
    return value ? value->type : JSON_ERROR;
}

TINY_LOR
JsonObject *json_value_get_object(const JsonElement *value)
{
    return json_value_get_type(value) == JSON_OBJECT ? value->value.object : NULL;
}

TINY_LOR
JsonArray *json_value_get_array(const JsonElement *value)
{
    return json_value_get_type(value) == JSON_ARRAY ? value->value.array : NULL;
}

TINY_LOR
const char *json_value_get_string(const JsonElement *value)
{
    return json_value_get_type(value) == JSON_STRING ? value->value.string : NULL;
}

TINY_LOR
double json_value_get_number(const JsonElement *value)
{
    return json_value_get_type(value) == JSON_NUMBER ? value->value.number : 0;
}

TINY_LOR
int json_value_get_boolean(const JsonElement *value)
{
    return json_value_get_type(value) == JSON_BOOLEAN ? value->value.boolean : -1;
}

//TINY_LOR
//JsonElement *json_value_get_parent(const JsonElement *value)
//{
//    return value ? value->parent : NULL;
//}

TINY_LOR
void json_value_free(JsonElement *value)
{
    switch (json_value_get_type(value))
    {
        case JSON_OBJECT:
            json_object_free(value->value.object);
            break;
        case JSON_STRING:
            tiny_free(value->value.string);
            break;
        case JSON_ARRAY:
            json_array_free(value->value.array);
            break;
        default:
            break;
    }
    tiny_free(value);
}

TINY_LOR
JsonElement *json_value_init_object(void)
{
    JsonElement *new_value = (JsonElement *) tiny_malloc(sizeof(JsonElement));
    if (!new_value)
    {
        return NULL;
    }
    new_value->parent = NULL;
    new_value->type = JSON_OBJECT;
    new_value->value.object = json_object_init(new_value);
    if (!new_value->value.object)
    {
        tiny_free(new_value);
        return NULL;
    }
    return new_value;
}

TINY_LOR
JsonElement *json_value_init_array(void)
{
    JsonElement *new_value = (JsonElement *) tiny_malloc(sizeof(JsonElement));
    if (!new_value)
    {
        return NULL;
    }
    new_value->parent = NULL;
    new_value->type = JSON_ARRAY;
    new_value->value.array = json_array_init(new_value);
    if (!new_value->value.array)
    {
        tiny_free(new_value);
        return NULL;
    }
    return new_value;
}

TINY_LOR
JsonElement *json_value_init_string(const char *string)
{
    char *copy = NULL;
    JsonElement *value;
    size_t string_len = 0;
    if (string == NULL)
    {
        return NULL;
    }
    string_len = strlen(string);
    if (!is_valid_utf8(string, string_len))
    {
        return NULL;
    }
    copy = parson_strndup(string, string_len);
    if (copy == NULL)
    {
        return NULL;
    }
    value = json_value_init_string_no_copy(copy);
    if (value == NULL)
    {
        tiny_free(copy);
    }
    return value;
}

TINY_LOR
JsonElement *json_value_init_number(double number)
{
    JsonElement *new_value = (JsonElement *) tiny_malloc(sizeof(JsonElement));
    if (!new_value)
    {
        return NULL;
    }
    new_value->parent = NULL;
    new_value->type = JSON_NUMBER;
    new_value->value.number = number;
    return new_value;
}

TINY_LOR
JsonElement *json_value_init_boolean(int boolean)
{
    JsonElement *new_value = (JsonElement *) tiny_malloc(sizeof(JsonElement));
    if (!new_value)
    {
        return NULL;
    }
    new_value->parent = NULL;
    new_value->type = JSON_BOOLEAN;
    new_value->value.boolean = boolean ? 1 : 0;
    return new_value;
}

TINY_LOR
JsonElement *json_value_init_null(void)
{
    JsonElement *new_value = (JsonElement *) tiny_malloc(sizeof(JsonElement));
    if (!new_value)
    {
        return NULL;
    }
    new_value->parent = NULL;
    new_value->type = JSON_NULL;
    return new_value;
}

TINY_LOR
JsonElement *json_value_deep_copy(const JsonElement *value)
{
    size_t i = 0;
    JsonElement *return_value = NULL, *temp_value_copy = NULL, *temp_value = NULL;
    const char *temp_string = NULL, *temp_key = NULL;
    char *temp_string_copy = NULL;
    JsonArray *temp_array = NULL, *temp_array_copy = NULL;
    JsonObject *temp_object = NULL, *temp_object_copy = NULL;

    switch (json_value_get_type(value))
    {
        case JSON_ARRAY:
            temp_array = json_value_get_array(value);
            return_value = json_value_init_array();
            if (return_value == NULL)
            {
                return NULL;
            }
            temp_array_copy = json_value_get_array(return_value);
            for (i = 0; i < json_array_get_count(temp_array); i++)
            {
                temp_value = json_array_get_value(temp_array, i);
                temp_value_copy = json_value_deep_copy(temp_value);
                if (temp_value_copy == NULL)
                {
                    json_value_free(return_value);
                    return NULL;
                }
                if (json_array_add(temp_array_copy, temp_value_copy) == JSON_FAILURE)
                {
                    json_value_free(return_value);
                    json_value_free(temp_value_copy);
                    return NULL;
                }
            }
            return return_value;
        case JSON_OBJECT:
            temp_object = json_value_get_object(value);
            return_value = json_value_init_object();
            if (return_value == NULL)
            {
                return NULL;
            }
            temp_object_copy = json_value_get_object(return_value);
            for (i = 0; i < json_object_get_count(temp_object); i++)
            {
                temp_key = json_object_get_name(temp_object, i);
                temp_value = json_object_get_value(temp_object, temp_key);
                temp_value_copy = json_value_deep_copy(temp_value);
                if (temp_value_copy == NULL)
                {
                    json_value_free(return_value);
                    return NULL;
                }
                if (json_object_add(temp_object_copy, temp_key, temp_value_copy) == JSON_FAILURE)
                {
                    json_value_free(return_value);
                    json_value_free(temp_value_copy);
                    return NULL;
                }
            }
            return return_value;
        case JSON_BOOLEAN:
            return json_value_init_boolean(json_value_get_boolean(value));
        case JSON_NUMBER:
            return json_value_init_number(json_value_get_number(value));
        case JSON_STRING:
            temp_string = json_value_get_string(value);
            if (temp_string == NULL)
            {
                return NULL;
            }
            temp_string_copy = parson_strdup(temp_string);
            if (temp_string_copy == NULL)
            {
                return NULL;
            }
            return_value = json_value_init_string_no_copy(temp_string_copy);
            if (return_value == NULL)
            {
                tiny_free(temp_string_copy);
            }
            return return_value;
        case JSON_NULL:
            return json_value_init_null();
        case JSON_ERROR:
            return NULL;
        default:
            return NULL;
    }
}

//TINY_LOR
//size_t json_serialization_size(const JsonElement *value)
//{
//    char num_buf[1100]; /* recursively allocating buffer on stack is a bad idea, so let's do it only once */
//    int res = json_serialize_to_buffer_r(value, NULL, 0, 0, num_buf);
//    return res < 0 ? 0 : (size_t) (res + 1);
//}

//TINY_LOR
//JsonResult json_serialize_to_buffer(const JsonElement *value, char *buf, size_t buf_size_in_bytes)
//{
//    int written = -1;
//    size_t needed_size_in_bytes = json_serialization_size(value);
//    if (needed_size_in_bytes == 0 || buf_size_in_bytes < needed_size_in_bytes)
//    {
//        return JSON_FAILURE;
//    }
//    written = json_serialize_to_buffer_r(value, buf, 0, 0, NULL);
//    if (written < 0)
//    {
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}

//TINY_LOR
//char *json_serialize_to_string(const JsonElement *value)
//{
//    JsonResult serialization_result = JSON_FAILURE;
//    size_t buf_size_bytes = json_serialization_size(value);
//    char *buf = NULL;
//    if (buf_size_bytes == 0)
//    {
//        return NULL;
//    }
//    buf = (char *) tiny_malloc(buf_size_bytes);
//    if (buf == NULL)
//    {
//        return NULL;
//    }
//    serialization_result = json_serialize_to_buffer(value, buf, buf_size_bytes);
//    if (serialization_result == JSON_FAILURE)
//    {
//        json_free_serialized_string(buf);
//        return NULL;
//    }
//    return buf;
//}

TINY_LOR
size_t json_serialization_size_pretty(const JsonElement *value)
{
    char num_buf[1100]; /* recursively allocating buffer on stack is a bad idea, so let's do it only once */
    int res = json_serialize_to_buffer_r(value, NULL, 0, 1, num_buf);
    return res < 0 ? 0 : (size_t) (res + 1);
}

TINY_LOR
JsonResult json_serialize_to_buffer_pretty(const JsonElement *value, char *buf, size_t buf_size_in_bytes)
{
    int written = -1;
    size_t needed_size_in_bytes = json_serialization_size_pretty(value);
    if (needed_size_in_bytes == 0 || buf_size_in_bytes < needed_size_in_bytes)
    {
        return JSON_FAILURE;
    }
    written = json_serialize_to_buffer_r(value, buf, 0, 1, NULL);
    if (written < 0)
    {
        return JSON_FAILURE;
    }
    return JSON_SUCCESS;
}

TINY_LOR
char *json_serialize_to_string_pretty(const JsonElement *value)
{
    JsonResult serialization_result = JSON_FAILURE;
    size_t buf_size_bytes = json_serialization_size_pretty(value);
    char *buf = NULL;
    if (buf_size_bytes == 0)
    {
        return NULL;
    }
    buf = (char *) tiny_malloc(buf_size_bytes);
    if (buf == NULL)
    {
        return NULL;
    }
    serialization_result = json_serialize_to_buffer_pretty(value, buf, buf_size_bytes);
    if (serialization_result == JSON_FAILURE)
    {
        json_free_serialized_string(buf);
        return NULL;
    }
    return buf;
}

TINY_LOR
void json_free_serialized_string(char *string)
{
    tiny_free(string);
}

//TINY_LOR
//JsonResult json_array_remove(JsonArray *array, size_t ix)
//{
//    JsonElement *temp_value = NULL;
//    size_t last_element_ix = 0;
//    if (array == NULL || ix >= json_array_get_count(array))
//    {
//        return JSON_FAILURE;
//    }
//    last_element_ix = json_array_get_count(array) - 1;
//    json_value_free(json_array_get_value(array, ix));
//    if (ix != last_element_ix)
//    { /* Replace value with one from the end of array */
//        temp_value = json_array_get_value(array, last_element_ix);
//        if (temp_value == NULL)
//        {
//            return JSON_FAILURE;
//        }
//        array->items[ix] = temp_value;
//    }
//    array->count -= 1;
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_replace_value(JsonArray *array, size_t ix, JsonElement *value)
//{
//    if (array == NULL || value == NULL || value->parent != NULL || ix >= json_array_get_count(array))
//    {
//        return JSON_FAILURE;
//    }
//    json_value_free(json_array_get_value(array, ix));
//    value->parent = json_array_get_wrapping_value(array);
//    array->items[ix] = value;
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_replace_string(JsonArray *array, size_t i, const char *string)
//{
//    JsonElement *value = json_value_init_string(string);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_array_replace_value(array, i, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_replace_number(JsonArray *array, size_t i, double number)
//{
//    JsonElement *value = json_value_init_number(number);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_array_replace_value(array, i, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_replace_boolean(JsonArray *array, size_t i, int boolean)
//{
//    JsonElement *value = json_value_init_boolean(boolean);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_array_replace_value(array, i, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_replace_null(JsonArray *array, size_t i)
//{
//    JsonElement *value = json_value_init_null();
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_array_replace_value(array, i, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_clear(JsonArray *array)
//{
//    size_t i = 0;
//    if (array == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    for (i = 0; i < json_array_get_count(array); i++)
//    {
//        json_value_free(json_array_get_value(array, i));
//    }
//    array->count = 0;
//    return JSON_SUCCESS;
//}

TINY_LOR
JsonResult json_array_append_value(JsonArray *array, JsonElement *value)
{
    if (array == NULL || value == NULL || value->parent != NULL)
    {
        return JSON_FAILURE;
    }
    return json_array_add(array, value);
}

TINY_LOR
JsonResult json_array_append_string(JsonArray *array, const char *string)
{
    JsonElement *value = json_value_init_string(string);
    if (value == NULL)
    {
        return JSON_FAILURE;
    }
    if (json_array_append_value(array, value) == JSON_FAILURE)
    {
        json_value_free(value);
        return JSON_FAILURE;
    }
    return JSON_SUCCESS;
}

//TINY_LOR
//JsonResult json_array_append_number(JsonArray *array, double number)
//{
//    JsonElement *value = json_value_init_number(number);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_array_append_value(array, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_append_boolean(JsonArray *array, int boolean)
//{
//    JsonElement *value = json_value_init_boolean(boolean);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_array_append_value(array, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_array_append_null(JsonArray *array)
//{
//    JsonElement *value = json_value_init_null();
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_array_append_value(array, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}

TINY_LOR
JsonResult json_object_set_value(JsonObject *object, const char *name, JsonElement *value)
{
    size_t i = 0;
    JsonElement *old_value;
    if (object == NULL || name == NULL || value == NULL || value->parent != NULL)
    {
        return JSON_FAILURE;
    }
    old_value = json_object_get_value(object, name);
    if (old_value != NULL)
    { /* free and overwrite old value */
        json_value_free(old_value);
        for (i = 0; i < json_object_get_count(object); i++)
        {
            if (strcmp(object->names[i], name) == 0)
            {
                value->parent = json_object_get_wrapping_value(object);
                object->values[i] = value;
                return JSON_SUCCESS;
            }
        }
    }
    /* add new key value pair */
    return json_object_add(object, name, value);
}

TINY_LOR
JsonResult json_object_set_string(JsonObject *object, const char *name, const char *string)
{
    return json_object_set_value(object, name, json_value_init_string(string));
}

TINY_LOR
JsonResult json_object_set_number(JsonObject *object, const char *name, double number)
{
    return json_object_set_value(object, name, json_value_init_number(number));
}

TINY_LOR
JsonResult json_object_set_boolean(JsonObject *object, const char *name, int boolean)
{
    return json_object_set_value(object, name, json_value_init_boolean(boolean));
}

//TINY_LOR
//JsonResult json_object_set_null(JsonObject *object, const char *name)
//{
//    return json_object_set_value(object, name, json_value_init_null());
//}

TINY_LOR
JsonResult json_object_dotset_value(JsonObject *object, const char *name, JsonElement *value)
{
    const char *dot_pos = NULL;
    char *current_name = NULL;
    JsonObject *temp_obj = NULL;
    JsonElement *new_value = NULL;
    if (value == NULL || name == NULL || value == NULL)
    {
        return JSON_FAILURE;
    }
    dot_pos = strchr(name, '.');
    if (dot_pos == NULL)
    {
        return json_object_set_value(object, name, value);
    }
    else
    {
        current_name = parson_strndup(name, dot_pos - name);
        temp_obj = json_object_get_object(object, current_name);
        if (temp_obj == NULL)
        {
            new_value = json_value_init_object();
            if (new_value == NULL)
            {
                tiny_free(current_name);
                return JSON_FAILURE;
            }
            if (json_object_add(object, current_name, new_value) == JSON_FAILURE)
            {
                json_value_free(new_value);
                tiny_free(current_name);
                return JSON_FAILURE;
            }
            temp_obj = json_object_get_object(object, current_name);
        }
        tiny_free(current_name);
        return json_object_dotset_value(temp_obj, dot_pos + 1, value);
    }
}

//TINY_LOR
//JsonResult json_object_dotset_string(JsonObject *object, const char *name, const char *string)
//{
//    JsonElement *value = json_value_init_string(string);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_object_dotset_value(object, name, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_object_dotset_number(JsonObject *object, const char *name, double number)
//{
//    JsonElement *value = json_value_init_number(number);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_object_dotset_value(object, name, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_object_dotset_boolean(JsonObject *object, const char *name, int boolean)
//{
//    JsonElement *value = json_value_init_boolean(boolean);
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_object_dotset_value(object, name, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}
//
//TINY_LOR
//JsonResult json_object_dotset_null(JsonObject *object, const char *name)
//{
//    JsonElement *value = json_value_init_null();
//    if (value == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    if (json_object_dotset_value(object, name, value) == JSON_FAILURE)
//    {
//        json_value_free(value);
//        return JSON_FAILURE;
//    }
//    return JSON_SUCCESS;
//}

TINY_LOR
JsonResult json_object_remove(JsonObject *object, const char *name)
{
    size_t i = 0, last_item_index = 0;
    if (object == NULL || json_object_get_value(object, name) == NULL)
    {
        return JSON_FAILURE;
    }
    last_item_index = json_object_get_count(object) - 1;
    for (i = 0; i < json_object_get_count(object); i++)
    {
        if (strcmp(object->names[i], name) == 0)
        {
            tiny_free(object->names[i]);
            json_value_free(object->values[i]);
            if (i != last_item_index)
            { /* Replace key value pair with one from the end */
                object->names[i] = object->names[last_item_index];
                object->values[i] = object->values[last_item_index];
            }
            object->count -= 1;
            return JSON_SUCCESS;
        }
    }
    return JSON_FAILURE; /* No execution path should end here */
}

TINY_LOR
JsonResult json_object_dotremove(JsonObject *object, const char *name)
{
    const char *dot_pos = strchr(name, '.');
    char *current_name = NULL;
    JsonObject *temp_obj = NULL;
    if (dot_pos == NULL)
    {
        return json_object_remove(object, name);
    }
    else
    {
        current_name = parson_strndup(name, dot_pos - name);
        temp_obj = json_object_get_object(object, current_name);
        tiny_free(current_name);
        if (temp_obj == NULL)
        {
            return JSON_FAILURE;
        }
        return json_object_dotremove(temp_obj, dot_pos + 1);
    }
}

//TINY_LOR
//JsonResult json_object_clear(JsonObject *object)
//{
//    size_t i = 0;
//    if (object == NULL)
//    {
//        return JSON_FAILURE;
//    }
//    for (i = 0; i < json_object_get_count(object); i++)
//    {
//        tiny_free(object->names[i]);
//        json_value_free(object->values[i]);
//    }
//    object->count = 0;
//    return JSON_SUCCESS;
//}

TINY_LOR
JsonResult json_validate(const JsonElement *schema, const JsonElement *value)
{
    JsonElement *temp_schema_value = NULL, *temp_value = NULL;
    JsonArray *schema_array = NULL, *value_array = NULL;
    JsonObject *schema_object = NULL, *value_object = NULL;
    JsonElementType schema_type = JSON_ERROR, value_type = JSON_ERROR;
    const char *key = NULL;
    size_t i = 0, count = 0;
    if (schema == NULL || value == NULL)
    {
        return JSON_FAILURE;
    }
    schema_type = json_value_get_type(schema);
    value_type = json_value_get_type(value);
    if (schema_type != value_type && schema_type != JSON_NULL)
    { /* null represents all values */
        return JSON_FAILURE;
    }
    switch (schema_type)
    {
        case JSON_ARRAY:
            schema_array = json_value_get_array(schema);
            value_array = json_value_get_array(value);
            count = json_array_get_count(schema_array);
            if (count == 0)
            {
                return JSON_SUCCESS; /* Empty array allows all types */
            }
            /* Get first value from array, rest is ignored */
            temp_schema_value = json_array_get_value(schema_array, 0);
            for (i = 0; i < json_array_get_count(value_array); i++)
            {
                temp_value = json_array_get_value(value_array, i);
                if (json_validate(temp_schema_value, temp_value) == JSON_FAILURE)
                {
                    return JSON_FAILURE;
                }
            }
            return JSON_SUCCESS;
        case JSON_OBJECT:
            schema_object = json_value_get_object(schema);
            value_object = json_value_get_object(value);
            count = json_object_get_count(schema_object);
            if (count == 0)
            {
                return JSON_SUCCESS; /* Empty object allows all objects */
            }
            else if (json_object_get_count(value_object) < count)
            {
                return JSON_FAILURE; /* Tested object mustn't have less name-value pairs than schema */
            }
            for (i = 0; i < count; i++)
            {
                key = json_object_get_name(schema_object, i);
                temp_schema_value = json_object_get_value(schema_object, key);
                temp_value = json_object_get_value(value_object, key);
                if (temp_value == NULL)
                {
                    return JSON_FAILURE;
                }
                if (json_validate(temp_schema_value, temp_value) == JSON_FAILURE)
                {
                    return JSON_FAILURE;
                }
            }
            return JSON_SUCCESS;
        case JSON_STRING:
        case JSON_NUMBER:
        case JSON_BOOLEAN:
        case JSON_NULL:
            return JSON_SUCCESS; /* equality already tested before switch */
        case JSON_ERROR:
        default:
            return JSON_FAILURE;
    }
}

TINY_LOR
JsonResult json_value_equals(const JsonElement *a, const JsonElement *b)
{
    JsonObject *a_object = NULL, *b_object = NULL;
    JsonArray *a_array = NULL, *b_array = NULL;
    const char *a_string = NULL, *b_string = NULL;
    const char *key = NULL;
    size_t a_count = 0, b_count = 0, i = 0;
    JsonElementType a_type, b_type;
    a_type = json_value_get_type(a);
    b_type = json_value_get_type(b);
    if (a_type != b_type)
    {
        return 0;
    }
    switch (a_type)
    {
        case JSON_ARRAY:
            a_array = json_value_get_array(a);
            b_array = json_value_get_array(b);
            a_count = json_array_get_count(a_array);
            b_count = json_array_get_count(b_array);
            if (a_count != b_count)
            {
                return 0;
            }
            for (i = 0; i < a_count; i++)
            {
                if (!json_value_equals(json_array_get_value(a_array, i),
                                       json_array_get_value(b_array, i)))
                {
                    return 0;
                }
            }
            return 1;
        case JSON_OBJECT:
            a_object = json_value_get_object(a);
            b_object = json_value_get_object(b);
            a_count = json_object_get_count(a_object);
            b_count = json_object_get_count(b_object);
            if (a_count != b_count)
            {
                return 0;
            }
            for (i = 0; i < a_count; i++)
            {
                key = json_object_get_name(a_object, i);
                if (!json_value_equals(json_object_get_value(a_object, key),
                                       json_object_get_value(b_object, key)))
                {
                    return 0;
                }
            }
            return 1;
        case JSON_STRING:
            a_string = json_value_get_string(a);
            b_string = json_value_get_string(b);
            if (a_string == NULL || b_string == NULL)
            {
                return 0; /* shouldn't happen */
            }
            return strcmp(a_string, b_string) == 0;
        case JSON_BOOLEAN:
            return json_value_get_boolean(a) == json_value_get_boolean(b);
        case JSON_NUMBER:
            return _fabs((float) (json_value_get_number(a) - json_value_get_number(b))) < 0.000001f; /* EPSILON */
        case JSON_ERROR:
            return 1;
        case JSON_NULL:
            return 1;
        default:
            return 1;
    }
}

//TINY_LOR
//JsonElementType json_type(const JsonElement *value)
//{
//    return json_value_get_type(value);
//}
//
//TINY_LOR
//JsonObject *json_object(const JsonElement *value)
//{
//    return json_value_get_object(value);
//}
//
//TINY_LOR
//JsonArray *json_array(const JsonElement *value)
//{
//    return json_value_get_array(value);
//}
//
//TINY_LOR
//const char *json_string(const JsonElement *value)
//{
//    return json_value_get_string(value);
//}
//
//TINY_LOR
//double json_number(const JsonElement *value)
//{
//    return json_value_get_number(value);
//}
//
//TINY_LOR
//int json_boolean(const JsonElement *value)
//{
//    return json_value_get_boolean(value);
//}