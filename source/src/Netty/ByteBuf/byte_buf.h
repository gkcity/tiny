
#ifndef __BYTE_BUF__
#define __BYTE_BUF__

#include <stdint.h>
#include "tiny_base.h"

#define BYTE_BUF_LEN 1024
#define BYTE_BUF_MAX 2048

TINY_BEGIN_DECLS
;
typedef struct _ByteBuf
{
    char* buf;
    uint32_t len;
    uint32_t readIndex;
    uint32_t writeIndex;
}ByteBuf;

ByteBuf* ByteBuf_New();
TinyRet ByteBuf_Construct(ByteBuf* thiz);
TinyRet ByteBuf_Write(ByteBuf* thiz, char* buf, int len);
bool ByteBuf_Writeable(ByteBuf* thiz);
int ByteBuf_Read(ByteBuf* thiz, char* buf, int len);
char ByteBuf_ReadByte(ByteBuf* thiz);
bool ByteBuf_Readable(ByteBuf* thiz);
int ByteBuf_Get(ByteBuf* thiz, char* buf, int len);
int ByteBuf_ReadableLen(ByteBuf* thiz);
char ByteBuf_GetByte(ByteBuf* thiz, int index);
int ByteBuf_GetBytes(ByteBuf* thiz, char* buf, int len);

/* for use ByteBuf buf use locally */
void ByteBuf_Handled(ByteBuf* thiz, int len);
char* ByteBuf_Buf(ByteBuf* thiz);


TinyRet ByteBuf_Clear(ByteBuf* thiz);
TinyRet ByteBuf_Dispose(ByteBuf *thiz);
void  ByteBuf_Delete(ByteBuf* thiz);

TINY_END_DECLS

#endif
