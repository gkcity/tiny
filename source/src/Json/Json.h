/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Json.h
 *
 * @remark
 *
 */

#ifndef __JSON_H__
#define __JSON_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


#if 0
/* JSON Types */
#define JSON_FALSE      0
#define JSON_TRUE       1
#define JSON_NULL       2
#define JSON_NUMBER     3
#define JSON_STRING     4
#define JSON_ARRAY      5
#define JSON_OBJECT     6
#endif

#define JSON_IsReference       256
#define JSON_StringIsConst     512

struct _Json;
typedef struct _Json Json;

typedef enum _JsonValueType
{
    JSON_BOOLEAN = 1,
    JSON_NULL = 2,
    JSON_NUMBER = 3,
    JSON_STRING = 4,
    JSON_ARRAY  = 5,
    JSON_OBJECT = 6,
} JsonValueType;

typedef union _JsonValueData
{


    char *string;			/* The item's string, if type==JSON_STRING */
    int integer;				/* The item's number, if type==JSON_NUMBER */
    double valuedouble;			/* The item's number, if type==JSON_NUMBER */

    char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} JsonValueData;

typedef struct _JsonValue
{
    JsonValueType type;
    JsonValueData data;
} JsonValue;

struct _Json
{
	Json *next;
    Json *prev;	        /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	Json *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

    char * name;
    JsonValue value;

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
};

typedef struct cJSON_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} cJSON_Hooks;

/* Supply malloc, realloc and free functions to Json */
extern void cJSON_InitHooks(cJSON_Hooks* hooks);


/* Supply a block of JSON, and this returns a Json object you can interrogate. Call cJSON_Delete when finished. */
extern Json *cJSON_Parse(const char *value);
/* Render a Json entity to text for transfer/storage. Free the char* when finished. */
extern char  *cJSON_Print(Json *item);
/* Render a Json entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *cJSON_PrintUnformatted(Json *item);
/* Render a Json entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
extern char *cJSON_PrintBuffered(Json *item,int prebuffer,int fmt);
/* Delete a Json entity and all subentities. */
extern void   cJSON_Delete(Json *c);

/* Returns the number of items in an array (or object). */
extern int	  cJSON_GetArraySize(Json *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern Json *cJSON_GetArrayItem(Json *array,int item);
/* Get item "string" from object. Case insensitive. */
extern Json *cJSON_GetObjectItem(Json *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
extern const char *cJSON_GetErrorPtr(void);
	
/* These calls create a Json item of the appropriate type. */
extern Json *cJSON_CreateNull(void);
extern Json *cJSON_CreateTrue(void);
extern Json *cJSON_CreateFalse(void);
extern Json *cJSON_CreateBool(int b);
extern Json *cJSON_CreateNumber(double num);
extern Json *cJSON_CreateString(const char *string);
extern Json *cJSON_CreateArray(void);
extern Json *cJSON_CreateObject(void);

/* These utilities create an Array of count items. */
extern Json *cJSON_CreateIntArray(const int *numbers,int count);
extern Json *cJSON_CreateFloatArray(const float *numbers,int count);
extern Json *cJSON_CreateDoubleArray(const double *numbers,int count);
extern Json *cJSON_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern void cJSON_AddItemToArray(Json *array, Json *item);
extern void	cJSON_AddItemToObject(Json *object,const char *string,Json *item);
extern void	cJSON_AddItemToObjectCS(Json *object,const char *string,Json *item);	/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the Json object */
/* Append reference to item to the specified array/object. Use this when you want to add an existing Json to a new Json, but don't want to corrupt your existing Json. */
extern void cJSON_AddItemReferenceToArray(Json *array, Json *item);
extern void	cJSON_AddItemReferenceToObject(Json *object,const char *string,Json *item);

/* Remove/Detatch items from Arrays/Objects. */
extern Json *cJSON_DetachItemFromArray(Json *array,int which);
extern void   cJSON_DeleteItemFromArray(Json *array,int which);
extern Json *cJSON_DetachItemFromObject(Json *object,const char *string);
extern void   cJSON_DeleteItemFromObject(Json *object,const char *string);
	
/* Update array items. */
extern void cJSON_InsertItemInArray(Json *array,int which,Json *newitem);	/* Shifts pre-existing items to the right. */
extern void cJSON_ReplaceItemInArray(Json *array,int which,Json *newitem);
extern void cJSON_ReplaceItemInObject(Json *object,const char *string,Json *newitem);

/* Duplicate a Json item */
extern Json *cJSON_Duplicate(Json *item,int recurse);
/* Duplicate will create a new, identical Json item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
extern Json *cJSON_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated);

extern void cJSON_Minify(char *json);

/* Macros for creating things quickly. */
#define cJSON_AddNullToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddBoolToObject(object,name,b)	cJSON_AddItemToObject(object, name, cJSON_CreateBool(b))
#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object,val)			((object)?(object)->valueint=(object)->valuedouble=(val):(val))
#define cJSON_SetNumberValue(object,val)		((object)?(object)->valueint=(object)->valuedouble=(val):(val))


TINY_END_DECLS

#endif /* __JSON_H__ */