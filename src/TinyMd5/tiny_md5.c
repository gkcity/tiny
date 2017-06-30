/**
 *
 * Copyright (C) 2007-2012 IGRSLab
 *
 * @author ouyangcf@igrslab.com
 * @date   2010-5-25
 *
 * @file   tiny_md5.c
 *
 * @version 2010.5.25
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */


#include "tiny_md5.h"  
#include <string.h>
#include <stdio.h>

/* POINTER defines a generic pointer urn */
typedef unsigned char *POINTER;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;
typedef unsigned char  UINT1;

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

typedef struct _TinyMD5
{
    UINT4 state[4]; /* state (ABCD) */
    UINT4 count[2]; /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64]; /* input buffer */
} TinyMD5;

static void TinyMD5_Init(TinyMD5 * thiz);
static void TinyMD5_Update(TinyMD5 * thiz, UINT1 * input, UINT4 inputLen);
static void TinyMD5_Final(TinyMD5 * thiz, UINT1 digest[16]);
static void TinyMD5_Transform(UINT4[4], unsigned char[64]);
static void TinyMD5_Encode(UINT1 *, UINT4 *, UINT4);
static void TinyMD5_Decode(UINT4 *, UINT1 *, UINT4);
static void TinyMD5_memcpy(UINT1 *, UINT1 *, UINT4);
static void TinyMD5_memset(UINT1 *, UINT1, UINT4);

static UINT1 PADDING[64] = 
{
    '\x80', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/* 
 * F, G, H and I are basic MD5 functions. 
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/*
 * rotate x left n bits
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
 * FF, GG, HH, and II tranformations for rounds 1, 2, 3 and 4.
 * ratation is separate from addition to prevent recomputation.
 */
#define XX(X, a, b, c, d, x, s, ac) { \
    (a) += X((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
    }
#define FF(a, b, c, d, x, s, ac) XX(F, a, b, c, d, x, s, ac)
#define GG(a, b, c, d, x, s, ac) XX(G, a, b, c, d, x, s, ac)
#define HH(a, b, c, d, x, s, ac) XX(H, a, b, c, d, x, s, ac)
#define II(a, b, c, d, x, s, ac) XX(I, a, b, c, d, x, s, ac)

/*
 * MD5 initialization. Begin an MD5 operation, writing a new context.
 */
static void TinyMD5_Init(TinyMD5 * thiz)
{
    thiz->count[0] = 0;
    thiz->count[1] = 0;
    thiz->state[0] = 0x67452301;
    thiz->state[1] = 0xefcdab89;
    thiz->state[2] = 0x98badcfe;
    thiz->state[3] = 0x10325476;
}

/* 
 * MD5 block update operation. Continues an MD5 message-digest
 * operation, processing another message block, and updating the
 * context.
 */
static void TinyMD5_Update(TinyMD5 * thiz, UINT1 * input, UINT4 inputLen)
{
    UINT4 i = 0;
    UINT4 index = 0;
    UINT4 partLen = 0;

    /* Compute number of bytes mod 64 */
    index = (UINT4)((thiz->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((thiz->count[0] += (inputLen << 3)) < (inputLen << 3))
    {
        thiz->count[1]++;
    }

    thiz->count[1] += (inputLen >> 29);
    partLen = 64 - index;

    /* 
     * Transform as many times as possible.
     */
    if (inputLen >= partLen) 
    {
        TinyMD5_memcpy(&thiz->buffer[index], input, partLen);
        TinyMD5_Transform(thiz->state, thiz->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
        {
            TinyMD5_Transform(thiz->state, &input[i]);
        }

        index = 0;
    }
    else
    {
        i = 0;    
    }

    /* Buffer remaining input */
    TinyMD5_memcpy(&thiz->buffer[index], &input[i], inputLen - i);
}

/* 
 * MD5 finalization. Ends an MD5 message-digest operation, writing the
 * the message digest and zeroizing the context.
 */
static void TinyMD5_Final(TinyMD5 * thiz, UINT1 digest[16])
{
    UINT1 bits[8];
    UINT4 index = 0;
    UINT4 padLen = 0;

    /* Save number of bits */
    TinyMD5_Encode(bits, thiz->count, 8);

    /* 
     * Pad out to 56 mod 64.
     */
    index = (UINT4)((thiz->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    TinyMD5_Update(thiz, PADDING, padLen);

    /* Append length (before padding) */
    TinyMD5_Update(thiz, bits, 8);

    /* Store state in digest */
    TinyMD5_Encode(digest, thiz->state, 16);

    /* Zeroize sensitive information.
    */
    TinyMD5_memset((POINTER)thiz, 0, sizeof(*thiz));
}

/* 
 * MD5 basic transformation. Transforms state based on block.
 */
static void TinyMD5_Transform(UINT4 state[4], UINT1 block[64])
{
    UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    TinyMD5_Decode(x, block, 64);

    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /*
     * Zeroize sensitive information.
     */
    TinyMD5_memset((POINTER)x, 0, sizeof (x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
 * a multiple of 4.
 */
static void TinyMD5_Encode(UINT1 * output, UINT4 * input, UINT4 len)
{
    UINT4 i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
 * a multiple of 4.
 */
static void TinyMD5_Decode(UINT4 * output, UINT1 * input, UINT4 len)
{
    UINT4 i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[i] = (input[j]) | 
                    ((input[j+1]) << 8) |
                    ((input[j+2]) << 16) | 
                    ((input[j+3]) << 24);
    }
}

/* 
 * Note: Replace "for loop" with standard memcpy if possible.
 */
static void TinyMD5_memcpy(UINT1 * output, UINT1 * input, UINT4 len)
{
    UINT4 i;

    for (i = 0; i < len; i++)
    {
        output[i] = input[i];
    }
}

/* 
 * Note: Replace "for loop" with standard memset if possible.
 */
static void TinyMD5_memset(UINT1 * output, UINT1 value, UINT4 len)
{
    UINT4 i;

    for (i = 0; i < len; i++)
    {
        ((UINT1 *)output)[i] = value;
    }
}

static void TinyMD5_Result(UINT1 digest[16], char result[40])
{
    UINT4 i;

    for (i = 0; i < 16; i++)
    {
        sprintf (&(result[i * 2]), "%02x", digest[i]);
    }

    return ;
}

void tiny_md5_encode(const char* src, char result[40])
{
    TinyMD5 thiz;
    UINT1 digest[16];
    UINT4 len = (UINT4) strlen(src);

    TinyMD5_Init(&thiz);
    TinyMD5_Update(&thiz, (UINT1 *)src, len);
    TinyMD5_Final(&thiz, digest);

    TinyMD5_Result(digest, result);

    return;
}