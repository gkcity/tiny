/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifdef LWIP_SOCKET
    #include "tiny_socket_lwip.c"
#else
    #ifdef WIN32
        #include "tiny_socket_windows.c"
    #else
        #include "tiny_socket_linux.c"
    #endif
#endif
