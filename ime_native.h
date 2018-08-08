/**
*
* @file
* @copyright
*                               --- WARNING ---
*
*     This work contains trade secrets of DataDirect Networks, Inc.  Any
*     unauthorized use or disclosure of the work, or any part thereof, is
*     strictly prohibited.  Copyright in this work is the property of
*     DataDirect Networks.  All Rights Reserved.  In the event of publication,
*     the following notice shall apply:  Copyright 2012-2014, DataDirect
*     Networks.
*
*
* @author Paul Nowoczynski <pauln@ddn.com>
* @section DESCRIPTION
*
*
*  Name: im_client_intercept.h
*  Module: client intercept
*  Project: Iron Monkey
*
*
*  Description: provides API for system call interception of common I/O
*               routines.
*
*
*  Created: Paul Nowoczynski
*
*
*
******************************************************************************/
#ifndef _IME_NATIVE_H
#define _IME_NATIVE_H

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define DEFAULT_IME_FILE_PREFIX     "ime://"
#define DEFAULT_IME_PREFIX_NO_FWD_SLASH "ime:"
#define DEFAULT_IME_FILE_PREFIX_LEN (sizeof(DEFAULT_IME_FILE_PREFIX) - 1)
#define IME_FILE_PREFIX_LEN_NO_FWD_SLASH (sizeof(DEFAULT_IME_PREFIX_NO_FWD_SLASH) - 1)

/* DE25626: remove one slash from the prefix because the absolute path
 * already contains one */
#define IME_FILE_PREFIX_LEN_ONE_SLASH (IME_FILE_PREFIX_LEN_NO_FWD_SLASH + 1)

void    ime_native_init(void);
int     ime_native_open(const char *, int, mode_t);
int     ime_native_close(int);
ssize_t ime_native_pwrite(int, const char *, size_t, off_t);
ssize_t ime_native_pwritev(int, const struct iovec *, int, off_t);
ssize_t ime_native_pread(int, char *, size_t, off_t);
ssize_t ime_native_preadv(int, const struct iovec *, int, off_t);
off_t   ime_native_lseek(int, off_t, int);
int     ime_native_finalize(void);
int     ime_native_stat(const char *, struct stat *);
int     ime_native_unlink(const char *);
int     ime_native_fsync(int);
int     ime_native_ftruncate(int fd, off_t off);

struct ime_aiocb
{
    int           fd;
    int           iovcnt;
    uint32_t      flags;
    const struct iovec *iov;
    off_t         file_offset;
    void        (*complete_cb)(struct ime_aiocb *aiocb, int err, ssize_t bytes);
    intptr_t      user_context;
};

int ime_native_aio_read(struct ime_aiocb *aiocb);
int ime_native_aio_write(struct ime_aiocb *aiocb);

#endif
