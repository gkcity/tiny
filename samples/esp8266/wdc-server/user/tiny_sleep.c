/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_sleep.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_sleep.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void tiny_sleep(int ms)
{
    printf("tiny_sleep: %d\n", ms);
}