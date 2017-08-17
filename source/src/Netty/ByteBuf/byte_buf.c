#include <stdlib.h>
#include <stdint.h>
#include "tiny_malloc.h"
#include "byte_buf.h"
#include "tiny_log.h"

#define TAG "ByteBuf"

TinyRet ByteBuf_Dispose(ByteBuf *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "ByteBuf_Dispose");

    if (thiz->buf != NULL)
    	tiny_free(thiz->buf);
    thiz->buf = NULL;
    thiz->len = 0;
    thiz->readIndex = 0;
    thiz->writeIndex = 0;

    return TINY_RET_OK;
}

void ByteBuf_Delete(ByteBuf* thiz)
{
    ByteBuf_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet ByteBuf_Construct(ByteBuf* thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "ByteBuf_Construct");

    do
    {
        memset(thiz, 0, sizeof(ByteBuf));

        char* buf = tiny_malloc(BYTE_BUF_LEN);
        if (buf == NULL)
        {
            LOG_E(TAG, "ByteBuf malloc error");
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
        thiz->buf = buf;
        thiz->len = BYTE_BUF_LEN;
        thiz->readIndex = 0;
        thiz->writeIndex = 0;
    } while (0);

    return ret;
}



ByteBuf* ByteBuf_New()
{
    ByteBuf *thiz = NULL;

    do
    {
        thiz = (ByteBuf *)tiny_malloc(sizeof(ByteBuf));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(ByteBuf_Construct(thiz)))
        {
            ByteBuf_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet ByteBuf_Write(ByteBuf * thiz,char * buf,int len)
{
    int ret = TINY_RET_OK;
    int remain = 0;
    char* tmp = NULL;
    
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_INVALID);
    LOG_D(TAG, "ByteBuf_Write");

    do
    {
        remain = thiz->len - thiz->writeIndex;
        if (remain >= len)
        {
            memcpy(thiz->buf + thiz->writeIndex, buf, len);
            thiz->writeIndex += len;
            break;
        }
        remain = thiz->len - thiz->readIndex + thiz->writeIndex;
        if (remain >= len)
        {
            memcpy(thiz->buf, thiz->buf + thiz->readIndex, thiz->writeIndex - thiz->readIndex);
            thiz->writeIndex -= thiz->readIndex;
            thiz->readIndex = 0;
            memcpy(thiz->buf + thiz->writeIndex, buf, len);
            thiz->writeIndex += len;
            break;
        }
        remain = len + thiz->writeIndex - thiz->readIndex;
        tmp = tiny_malloc(remain);
        if (tmp == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            LOG_E(TAG, "malloc while ByteBuf_Write");
            break;
        }
        memcpy(tmp, thiz->buf + thiz->readIndex, thiz->writeIndex - thiz->readIndex);
        tiny_free(thiz->buf);
        thiz->buf = tmp;
        thiz->len = remain;
        thiz->writeIndex -= thiz->readIndex;
        thiz->readIndex = 0;
	memcpy(thiz->buf + thiz->writeIndex, buf, len);
        thiz->writeIndex += len;
            
    }while(0);

    return ret;
}

bool ByteBuf_Writeable(ByteBuf * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);
    return thiz->len < BYTE_BUF_MAX;
}

bool ByteBuf_Readable(ByteBuf * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);
    return thiz->writeIndex > thiz->readIndex;
}

int ByteBuf_Read(ByteBuf * thiz,char * buf,int len)
{
    int ret = -1;
    int readableLen = 0;

    RETURN_VAL_IF_FAIL(thiz, -1);
    RETURN_VAL_IF_FAIL(buf, -1);
    if (len <= 0) return -1;
    
    do
    {
        readableLen = thiz->writeIndex - thiz->readIndex;
    	if (readableLen <= 0)
    	{
            LOG_W(TAG, "unreadable");
            break;
    	}
    	ret = readableLen > len ? len : readableLen;
    	memcpy(buf, thiz->buf + thiz->readIndex, ret);
    	thiz->readIndex += ret;
    	
    } while(0);
    
    return ret;
}

char ByteBuf_ReadByte(ByteBuf * thiz)
{
    char ret = -1;
    do
    {
        if (!ByteBuf_Readable(thiz))
        {
            break;
        }
        ret = thiz->buf[thiz->readIndex++];
    } while(0);
    return ret;
}

char ByteBuf_GetByte(ByteBuf* thiz, int index)
{
    char ret = -1;

    do
    {
        if (index < 0) break;
        if (thiz->readIndex + index >= thiz->writeIndex) break;
        ret = thiz->buf[thiz->readIndex + index];
    } while(0);
    return ret;
}

int ByteBuf_GetBytes(ByteBuf* thiz, char* buf, int len)
{
    int ret = -1;
    int readableLen = -1;
    RETURN_VAL_IF_FAIL(thiz, -1);
    RETURN_VAL_IF_FAIL(buf, -1);
    if (len <= 0) return -1;
    
    do
    {
        readableLen = thiz->writeIndex - thiz->readIndex;
    	if (readableLen <= 0)
    	{
            LOG_W(TAG, "unreadable");
            break;
    	}
    	ret = readableLen > len ? len : readableLen;
    	memcpy(buf, thiz->buf + thiz->readIndex, ret);
    	
    } while(0);
    
    return ret;
}

int ByteBuf_ReadableLen(ByteBuf * thiz)
{
    return thiz->writeIndex - thiz->readIndex;
}

TinyRet ByteBuf_Clear(ByteBuf* thiz)
{
    thiz->readIndex = 0;
    thiz->writeIndex = 0;
    return TINY_RET_OK;
}

void ByteBuf_Handled(ByteBuf* thiz, int len)
{
    if (len > ByteBuf_ReadableLen(thiz))
    {
        return;
    }
    thiz->readIndex += len;
}

char* ByteBuf_Buf(ByteBuf* thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    return thiz->buf + thiz->readIndex;
}