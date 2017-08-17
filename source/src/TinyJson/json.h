/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyJson.h
 *
 * @remark
 *
 */

#ifndef __TINY_JSON_H__
#define __TINY_JSON_H__

#include <tiny_lor.h>
#include "tiny_base.h"

TINY_BEGIN_DECLS


struct _JsonObject;
struct _JsonArray;
struct _JsonElement;

typedef struct _JsonObject JsonObject;
typedef struct _JsonArray JsonArray;
typedef struct _JsonElement JsonElement;

typedef enum _JsonElementType
{
    JSON_ERROR = -1,
    JSON_NULL = 1,
    JSON_STRING = 2,
    JSON_NUMBER = 3,
    JSON_OBJECT = 4,
    JSON_ARRAY = 5,
    JSON_BOOLEAN = 6
} JsonElementType;

typedef enum _JsonResult
{
    JSON_SUCCESS = 0,
    JSON_FAILURE = -1
} JsonResult;

/*  Parses first JSON value in a string, returns NULL in case of error */
TINY_LOR
JsonElement *json_parse_string(const char *string);

/* Serialization */
//TINY_LOR
//size_t json_serialization_size(const JsonElement *value); /* returns 0 on fail */

//TINY_LOR
//JsonResult json_serialize_to_buffer(const JsonElement *value, char *buf, size_t buf_size_in_bytes);

//JsonResult json_serialize_to_file(const JsonElement *value, const char *filename);

//TINY_LOR
//char *json_serialize_to_string(const JsonElement *value);

/* Pretty serialization */
TINY_LOR
size_t json_serialization_size_pretty(const JsonElement *value); /* returns 0 on fail */

TINY_LOR
JsonResult json_serialize_to_buffer_pretty(const JsonElement *value, char *buf, size_t buf_size_in_bytes);

TINY_LOR
char *json_serialize_to_string_pretty(const JsonElement *value);

/* frees string from json_serialize_to_string and json_serialize_to_string_pretty */
TINY_LOR
void json_free_serialized_string(char *string);

/* Comparing */
TINY_LOR
JsonResult json_value_equals(const JsonElement *a, const JsonElement *b);

/* Validation
   This is *NOT* JSON Schema. It validates TinyJson by checking if object have identically
   named fields with matching types.
   For example-httpserver schema {"name":"", "age":0} will validate
   {"name":"Joe", "age":25} and {"name":"Joe", "age":25, "gender":"m"},
   but not {"name":"Joe"} or {"name":"Joe", "age":"Cucumber"}.
   In case of arrays, only first value in schema is checked against all values in tested array.
   Empty objects ({}) validate all objects, empty arrays ([]) validate all arrays,
   null validates values of every type.
 */
TINY_LOR
JsonResult json_validate(const JsonElement *schema, const JsonElement *value);

/*
 * JSON Object
 */
TINY_LOR
JsonElement *json_object_get_value(const JsonObject *object, const char *name);

TINY_LOR
const char *json_object_get_string(const JsonObject *object, const char *name);

TINY_LOR
JsonObject *json_object_get_object(const JsonObject *object, const char *name);

TINY_LOR
JsonArray *json_object_get_array(const JsonObject *object, const char *name);

TINY_LOR
double json_object_get_number(const JsonObject *object, const char *name); /* returns 0 on fail */

TINY_LOR
int json_object_get_boolean(const JsonObject *object, const char *name); /* returns -1 on fail */

/* dotget functions enable addressing values with dot notation in nested objects,
 just like in structs or c++/java/c# objects (e.g. objectA.objectB.value).
 Because valid names in JSON can contain dots, some values may be inaccessible
 this way. */
TINY_LOR
JsonElement *json_object_dotget_value(const JsonObject *object, const char *name);

//TINY_LOR
//const char *json_object_dotget_string(const JsonObject *object, const char *name);
//
//TINY_LOR
//JsonObject *json_object_dotget_object(const JsonObject *object, const char *name);
//
//TINY_LOR
//JsonArray *json_object_dotget_array(const JsonObject *object, const char *name);
//
//TINY_LOR
//double json_object_dotget_number(const JsonObject *object, const char *name); /* returns 0 on fail */
//
//TINY_LOR
//int json_object_dotget_boolean(const JsonObject *object, const char *name); /* returns -1 on fail */

/* Functions to get available names */
TINY_LOR
size_t json_object_get_count(const JsonObject *object);

TINY_LOR
const char *json_object_get_name(const JsonObject *object, size_t index);

//TINY_LOR
//JsonElement *json_object_get_value_at(const JsonObject *object, size_t index);

TINY_LOR
JsonElement *json_object_get_wrapping_value(const JsonObject *object);

///* Functions to check if object has a value with a specific name. Returned value is 1 if object has
// * a value and 0 if it doesn't. dothas functions behave exactly like dotget functions. */
//TINY_LOR
//int json_object_has_value(const JsonObject *object, const char *name);

TINY_LOR
int json_object_has_value_of_type(const JsonObject *object, const char *name, JsonElementType type);

//TINY_LOR
//int json_object_dothas_value(const JsonObject *object, const char *name);

//TINY_LOR
//int json_object_dothas_value_of_type(const JsonObject *object, const char *name, JsonElementType type);

/* Creates new name-value pair or frees and replaces old value with a new one.
 * json_object_set_value does not copy passed value so it shouldn't be freed afterwards. */
TINY_LOR
JsonResult json_object_set_value(JsonObject *object, const char *name, JsonElement *value);

TINY_LOR
JsonResult json_object_set_string(JsonObject *object, const char *name, const char *string);

TINY_LOR
JsonResult json_object_set_number(JsonObject *object, const char *name, double number);

TINY_LOR
JsonResult json_object_set_boolean(JsonObject *object, const char *name, int boolean);

//TINY_LOR
//JsonResult json_object_set_null(JsonObject *object, const char *name);

/* Works like dotget functions, but creates whole hierarchy if necessary.
 * json_object_dotset_value does not copy passed value so it shouldn't be freed afterwards. */
TINY_LOR
JsonResult json_object_dotset_value(JsonObject *object, const char *name, JsonElement *value);

//TINY_LOR
//JsonResult json_object_dotset_string(JsonObject *object, const char *name, const char *string);
//
//TINY_LOR
//JsonResult json_object_dotset_number(JsonObject *object, const char *name, double number);
//
//TINY_LOR
//JsonResult json_object_dotset_boolean(JsonObject *object, const char *name, int boolean);
//
//TINY_LOR
//JsonResult json_object_dotset_null(JsonObject *object, const char *name);

/* Frees and removes name-value pair */
TINY_LOR JsonResult json_object_remove(JsonObject *object, const char *name);

/* Works like dotget function, but removes name-value pair only on exact match. */
TINY_LOR JsonResult json_object_dotremove(JsonObject *object, const char *key);

/* Removes all name-value pairs in object */
//TINY_LOR JsonResult json_object_clear(JsonObject *object);

/*
 *JSON Array
 */
TINY_LOR
JsonElement *json_array_get_value(const JsonArray *array, size_t index);

//TINY_LOR
//const char *json_array_get_string(const JsonArray *array, size_t index);

TINY_LOR
JsonObject *json_array_get_object(const JsonArray *array, size_t index);

//TINY_LOR
//JsonArray *json_array_get_array(const JsonArray *array, size_t index);
//
//TINY_LOR
//double json_array_get_number(const JsonArray *array, size_t index); /* returns 0 on fail */
//
//TINY_LOR
//int json_array_get_boolean(const JsonArray *array, size_t index); /* returns -1 on fail */

TINY_LOR
size_t json_array_get_count(const JsonArray *array);

TINY_LOR
JsonElement *json_array_get_wrapping_value(const JsonArray *array);

///* Frees and removes value at given index, does nothing and returns JSON_FAILURE if index doesn't exist.
// * Order of values in array may change during execution.  */
//TINY_LOR
//JsonResult json_array_remove(JsonArray *array, size_t i);

/* Frees and removes from array value at given index and replaces it with given one.
 * Does nothing and returns JSON_FAILURE if index doesn't exist.
 * json_array_replace_value does not copy passed value so it shouldn't be freed afterwards. */
//TINY_LOR
//JsonResult json_array_replace_value(JsonArray *array, size_t i, JsonElement *value);
//
//TINY_LOR
//JsonResult json_array_replace_string(JsonArray *array, size_t i, const char *string);
//
//TINY_LOR
//JsonResult json_array_replace_number(JsonArray *array, size_t i, double number);
//
//TINY_LOR
//JsonResult json_array_replace_boolean(JsonArray *array, size_t i, int boolean);
//
//TINY_LOR
//JsonResult json_array_replace_null(JsonArray *array, size_t i);
//
///* Frees and removes all values from array */
//TINY_LOR
//JsonResult json_array_clear(JsonArray *array);

/* Appends new value at the end of array.
 * json_array_append_value does not copy passed value so it shouldn't be freed afterwards. */
TINY_LOR
JsonResult json_array_append_value(JsonArray *array, JsonElement *value);

TINY_LOR
JsonResult json_array_append_string(JsonArray *array, const char *string);

//TINY_LOR
//JsonResult json_array_append_number(JsonArray *array, double number);
//
//TINY_LOR
//JsonResult json_array_append_boolean(JsonArray *array, int boolean);
//
//TINY_LOR
//JsonResult json_array_append_null(JsonArray *array);

/*
 *JSON Value
 */
TINY_LOR
JsonElement *json_value_init_object(void);

TINY_LOR
JsonElement *json_value_init_array(void);

TINY_LOR
JsonElement *json_value_init_string(const char *string); /* copies passed string */
TINY_LOR
JsonElement *json_value_init_number(double number);

TINY_LOR
JsonElement *json_value_init_boolean(int boolean);

TINY_LOR
JsonElement *json_value_init_null(void);

TINY_LOR
JsonElement *json_value_deep_copy(const JsonElement *value);

TINY_LOR
void json_value_free(JsonElement *value);

TINY_LOR
JsonElementType json_value_get_type(const JsonElement *value);

TINY_LOR
JsonObject *json_value_get_object(const JsonElement *value);

TINY_LOR
JsonArray *json_value_get_array(const JsonElement *value);

TINY_LOR
const char *json_value_get_string(const JsonElement *value);

TINY_LOR
double json_value_get_number(const JsonElement *value);

TINY_LOR
int json_value_get_boolean(const JsonElement *value);

//TINY_LOR
//JsonElement *json_value_get_parent(const JsonElement *value);
//
///* Same as above, but shorter */
//TINY_LOR
//JsonElementType json_type(const JsonElement *value);
//
//TINY_LOR
//JsonObject *json_object(const JsonElement *value);
//
//TINY_LOR
//JsonArray *json_array(const JsonElement *value);
//
//TINY_LOR
//const char *json_string(const JsonElement *value);
//
//TINY_LOR
//double json_number(const JsonElement *value);
//
//TINY_LOR
//int json_boolean(const JsonElement *value);


TINY_END_DECLS

#endif /* __TINY_JSON_H__ */