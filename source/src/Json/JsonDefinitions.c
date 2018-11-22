/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonDefinitions.c
 *
 * @remark
 *
 */

#include "JsonDefinitions.h"

const char * JsonValueType_ToString(JsonValueType type)
{
    switch (type)
    {
        case JSON_STRING:
            return "string";

        case JSON_NUMBER:
            return "number";

        case JSON_OBJECT:
            return "object";

        case JSON_ARRAY:
            return "array";

        case JSON_BOOLEAN:
            return "boolean";

        case JSON_NULL:
            return "null";

        default:
            return "undefined";
    }
}