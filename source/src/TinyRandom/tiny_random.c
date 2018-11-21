/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   random.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_time.h>
#include <tiny_log.h>
#include "tiny_random.h"

#define TAG   "tiny_random"

#ifndef ULONG_MAX
#define ULONG_MAX  ((unsigned long)(~0L))       /* 0xFFFFFFFF*/
#endif

#ifndef LONG_MAX
#define LONG_MAX   ((long)(ULONG_MAX >> 1))     /* 0x7FFFFFFF*/
#endif

#ifndef NULL
#define NULL (void *) 0
#endif

static long int _RandomTable[32] = {
        3,
        0x9a319039, 0x32d9c024, 0x9b663182, 0x5da1f342,
        0xde3b81e0, 0xdf0a6fb5, 0xf103bc02, 0x48f340fb,
        0x7449e56b, 0xbeb1dbb0, 0xab5c5918, 0x946554fd,
        0x8c2e680f, 0xeb3d799f, 0xb11ee0b7, 0x2d436b86,
        0xda672e2a, 0x1588ca88, 0xe369735d, 0x904f35f7,
        0xd7158fd6, 0x6fa6f051, 0x616e6b96, 0xac94efdc,
        0x36413f93, 0xc622c298, 0xf5a42ab8, 0x8a88d77b,
        0xf5ad9d0e, 0x8999220b, 0x27fb47b9
};

static long int *_fptr = &_RandomTable[4];
static long int *_rptr = &_RandomTable[1];
static long int *_state = &_RandomTable[1];
static long int *_end_ptr = &_RandomTable[sizeof(_RandomTable) / sizeof(_RandomTable[0])];

TINY_LOR
static long int _random()
{
    long int i = 0;

    *_fptr += *_rptr;

    i = (*_fptr >> 1) & LONG_MAX;

    _fptr++;

    if (_fptr >= _end_ptr)
    {
        _fptr = _state;
        _rptr++;
    }
    else
    {
        _rptr++;
        if (_rptr >= _end_ptr)
        {
            _rptr = _state;
        }
    }

    return i;
}

TINY_LOR
static void _srandom(int seed)
{
//    LOG_D(TAG, "_srandom: %d", seed);

    _state[0] = seed;
    register long int i;
    for (i = 1; i < 31; ++i)
    {
        _state[i] = (1103515145 * _state[i - 1]) + 12345;
    }

    _fptr = &_state[3];
    _rptr = &_state[0];

    for (i = 0; i < 10 * 31; ++i)
    {
        _random();
    }
}

TINY_LOR
static uint8_t _rand_get_available_bytes(void)
{
    _srandom((int) tiny_current_microsecond());

//#ifdef ESP
//    _srandom(* ((int *)_state));
//#else
//    _srandom(* ((int *)_state));
//    //_srandom((unsigned )tiny_time(NULL));
//#endif

    return 32;
}

TINY_LOR
static void _rand_get_vector(uint8_t *vector, uint8_t length)
{
    uint8_t i = 0;

    for (i = 0; i < length; ++i)
    {
        long int v = _random();

        vector[i] = (uint8_t)v;
    }
}

TINY_LOR
void tiny_random_create(uint8_t *p_result, uint8_t length)
{
    while (length)
    {
        uint8_t available = _rand_get_available_bytes();
        if (available == 0)
        {
            break;
        }

        available = available < length ? available : length;
        _rand_get_vector(p_result, available);
        p_result += available;
        length -= available;
    }
}