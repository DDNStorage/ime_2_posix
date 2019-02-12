/**
* Infinite Memory Engine - Native interface
*
* Copyright (c) 2018, DataDirect Networks.
******************************************************************************/

#ifndef _IME_NATIVE_H
#define _IME_NATIVE_H

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define DEFAULT_IME_FILE_PREFIX     "ime://"
#define DEFAULT_IME_PREFIX_NO_FWD_SLASH "ime:"
#define DEFAULT_IME_FILE_PREFIX_LEN (sizeof(DEFAULT_IME_FILE_PREFIX) - 1)
#define IME_FILE_PREFIX_LEN_NO_FWD_SLASH (sizeof(DEFAULT_IME_PREFIX_NO_FWD_SLASH) - 1)

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

void    ime_native_init(void);
int     ime_native_access(const char *pathname, int mode);
int     ime_native_open(const char *pathname, int flags, mode_t mode);
int     ime_native_close(int fd);
int     ime_native_mkdir(const char *pathname, mode_t mode);
int     ime_native_rmdir(const char *pathname);
int     ime_native_statvfs(const char *pathname, struct statvfs *buf);
ssize_t ime_native_write(int fd, const char *buf, size_t count);
ssize_t ime_native_read(int fd, char *buf, size_t count);
ssize_t ime_native_pwrite(int fd, const char *buf, size_t count, off_t offset);
ssize_t ime_native_pread(int fd, char *buf, size_t count, off_t offset);
ssize_t ime_native_pwritev(int fd, const struct iovec *iov,
                           int iovcnt, off_t offset);
ssize_t ime_native_preadv(int fd, const struct iovec *iov,
                          int iovcnt, off_t offset);
int     ime_native_aio_write(struct ime_aiocb *aiocb);
int     ime_native_aio_read(struct ime_aiocb *aiocb);
int     ime_native_dup2(int oldfd, int newfd);
off_t   ime_native_lseek(int fd, off_t offset, int whence);
int     ime_native_finalize(void);
int     ime_native_stat(const char *pathname, struct stat *buf);
int     ime_native_unlink(const char *pathname);
int     ime_native_fsync(int fd);
int     ime_native_ftruncate(int fd, off_t off);

#endif

