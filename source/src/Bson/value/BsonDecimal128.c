/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonDecimal128.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include "BsonDecimal128.h"

TINY_LOR
BsonDecimal128 * BsonDecimal128_New(const uint8_t *value)
{
    BsonDecimal128 *thiz = NULL;

//    do
//    {
//        thiz = (BsonDecimal128 *) tiny_malloc(sizeof(BsonDecimal128));
//        if (thiz == NULL)
//        {
//            LOG_E(TAG, "tiny_malloc FAILED!");
//            break;
//        }
//
//        if (RET_FAILED(BsonDecimal128_Construct(thiz, value)))
//        {
//            LOG_E(TAG, "BsonDecimal128_Construct FAILED");
//            BsonDecimal128_Delete(thiz);
//            thiz = NULL;
//            break;
//        }
//    } while (0);

    return thiz;
}

TINY_LOR
void BsonDecimal128_Delete(BsonDecimal128 *thiz)
{
    tiny_free(thiz);
}