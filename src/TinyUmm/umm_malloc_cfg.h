/**
 * Copyright (C) 2013-2015
 *
 * User-dependent configuration file for tiny_malloc
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_memory.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 */

#ifndef __UMM_MALLOC_CFG_H__
#define __UMM_MALLOC_CFG_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS

/**
 ----------------------------------------------------------------------------

 There are a number of defines you can set at compile time that affect how
 the memory allocator will operate.
 You can set them in your config file umm_malloc_cfg.h.
 In GNU C, you also can set these compile time defines like this:

 -D TINY_TEST_MAIN

 Set this if you want to compile in the test suite at the end of this file.

 If you leave this define unset, then you might want to set another one:

 -D TINY_REDEFINE_MEM_FUNCTIONS

 If you leave this define unset, then the function names are left alone as
 umm_malloc() umm_free() and umm_realloc() so that they cannot be confused
 with the C runtime functions malloc() free() and realloc()

 If you do set this define, then the function names become malloc()
 free() and realloc() so that they can be used as the C runtime functions
 in an embedded environment.

 -D TINY_BEST_FIT (defualt)

 Set this if you want to use a best-fit algorithm for allocating new
 blocks

 -D TINY_FIRST_FIT

 Set this if you want to use a first-fit algorithm for allocating new
 blocks

 -D TINY_DBG_LOG_LEVEL=n

 Set n to a value from 0 to 6 depending on how verbose you want the debug
 log to be

 If you want umm_malloc not to be built at all, without removing it from the project,
 you can set this macro:

 -D UMM_MALLOC_CFG__DONT_BUILD

 This can be useful if you want to easily switch between different allocators
 to test them.

 ----------------------------------------------------------------------------

 Support for this library in a multitasking environment is provided when
 you add bodies to the TINY_CRITICAL_ENTRY and TINY_CRITICAL_EXIT macros
 (see below)

 ----------------------------------------------------------------------------
*/


/**
 * Size of the heap in bytes
 */
#define UMM_MALLOC_CFG__HEAP_SIZE  (1024 * 16)

/**
 * A couple of macros to make packing structures less compiler dependent
 */
#define UMM_H_ATTPACKPRE
#define UMM_H_ATTPACKSUF __attribute__((__packed__))

/**
 * A couple of macros to make it easier to protect the memory allocator
 * in a multitasking system. You should set these macros up to use whatever
 * your system uses for this purpose. You can disable interrupts entirely, or
 * just disable task switching - it's up to you
 *
 * NOTE WELL that these macros MUST be allowed to nest, because umm_free() is
 * called from within umm_malloc()
 */

#define UMM_CRITICAL_ENTRY()
#define UMM_CRITICAL_EXIT()


TINY_END_DECLS

#endif /* __UMM_MALLOC_CFG_H__ */