/**
* Infinite Memory Engine - IME native wrapper
*
* Copyright (c) 2018, DataDirect Networks.
******************************************************************************/

#define _XOPEN_SOURCE 500 /* for pread/pwrite */
#define _DEFAULT_SOURCE /* for preadv/pwritev */

#include "ime_native.h"

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/uio.h>
#include <errno.h>
#include <strings.h>

#ifdef USE_LIO
#include <stdlib.h>
#include <aio.h>
#include <signal.h>

struct ime_data {
    struct ime_aiocb *aiocb;
    struct aiocb **aiocb_list;
    struct sigevent sigev;
};
#endif

const static char mock_version[] = "IME mock native API";

static const char *get_real_path(const char *pathname)
{
    return strncasecmp(DEFAULT_IME_FILE_PREFIX, pathname,
                       DEFAULT_IME_FILE_PREFIX_LEN) ?
                       pathname : pathname + DEFAULT_IME_FILE_PREFIX_LEN;
}

void ime_native_init(void)
{
    /* Nothing to do */
}

int ime_native_access(const char *pathname, int mode)
{
    const char* real_path = get_real_path(pathname);
    return access(real_path, mode);
}

int ime_native_open(const char *pathname, int amode, mode_t perm)
{
    int fd = -1;
    const char* real_path = get_real_path(pathname);

    if (real_path != NULL)
        fd = open (real_path, amode, perm);
    else
        errno = ENOENT;

    return fd;
}

int ime_native_close(int fd)
{
    return close(fd);
}

int ime_native_mkdir(const char *pathname, mode_t mode)
{
    const char* real_path = get_real_path(pathname);

    return mkdir(real_path, mode);
}

int ime_native_rmdir(const char *pathname)
{
    const char* real_path = get_real_path(pathname);

    return rmdir(real_path);
}

int ime_native_statvfs(const char *pathname, struct statvfs *buf)
{
    const char* real_path = get_real_path(pathname);

    return statvfs(real_path, buf);
}

ssize_t ime_native_write(int fd, const char *buf, size_t count)
{
    return write(fd, buf, count);
}

ssize_t ime_native_read(int fd, char *buf, size_t count)
{
    return read(fd, buf, count);
}

ssize_t ime_native_pwrite(int fd, const char *buf, size_t count, off_t offset)
{
    return pwrite(fd, buf, count, offset);
}

ssize_t ime_native_pread(int fd, char *buf, size_t count, off_t offset)
{
    return pread(fd, buf, count, offset);
}

ssize_t ime_native_pwritev(int fd, const struct iovec *iov,
                           int iovcnt, off_t offset)
{
    return pwritev(fd, iov, iovcnt, offset);
}

ssize_t ime_native_preadv(int fd, const struct iovec *iov,
                          int iovcnt, off_t offset)
{
    return preadv(fd, iov, iovcnt, offset);
}

int ime_native_dup2(int oldfd, int newfd)
{
    return dup2(oldfd, newfd);
}

off_t ime_native_lseek(int fd, off_t offset, int whence)
{
    return lseek(fd, offset, whence);
}

int ime_native_finalize(void)
{
    /* Nothing to do */
    return 0;
}

int ime_native_stat(const char *pathname, struct stat *statbuffer)
{
    const char* real_path = get_real_path(pathname);

    if (real_path == NULL) {
        errno = ENOENT;
        return -1;
    }
    return stat(real_path, statbuffer);
}

int ime_native_unlink(const char *pathname)
{
    const char* real_path = get_real_path(pathname);

    if (real_path == NULL) {
        errno = ENOENT;
        return -1;
    }
    return unlink(real_path);
}

int ime_native_fsync(int fd)
{
    return fsync(fd);
}

int ime_native_ftruncate(int fd, off_t offset)
{
    return ftruncate(fd, offset);
}

const char *ime_native_version(void)
{
    return mock_version;
}

#ifdef USE_LIO
static void aio_end_callback(union sigval val)
{
    ssize_t ret, bytes;
    struct ime_data *ime_d = val.sival_ptr;

    for (int i = 0; i < ime_d->aiocb->iovcnt; i++) {
        ret = aio_return(ime_d->aiocb_list[i]);
        if (ret < 0) {
            ime_d->aiocb->complete_cb(ime_d->aiocb, errno, ret);
            goto exit_free;
        }
        bytes += ret;
    }

    ime_d->aiocb->complete_cb(ime_d->aiocb, 0, bytes);

exit_free:
    free(ime_d->aiocb_list);
    free(ime_d);
}

static int aio_op(struct ime_aiocb *aiocb, int lio_opcode)
{
    if (aiocb->iovcnt == 0) {
        aiocb->complete_cb(aiocb, 0, 0);
        return 0;
    }

    /* allocate some memory to store the list IO requests */
    struct ime_data *ime_d = malloc(sizeof(struct ime_data));
    struct aiocb *aiocb_array;
    if (ime_d == NULL) {
        return -1;
    }
    ime_d->aiocb_list = malloc(aiocb->iovcnt *
                               (sizeof(struct aiocb*) + sizeof(struct aiocb)));
    if (ime_d->aiocb_list == NULL) {
        free(ime_d);
        return -1;
    }

    /* create the aiocb pointers list using the aiocbs array */
    aiocb_array = (struct aiocb*) (ime_d->aiocb_list + aiocb->iovcnt);
    for (int i = 0; i < aiocb->iovcnt; i++) {
        ime_d->aiocb_list[i] = &aiocb_array[i];
    }

    /* create the sigevent struct to use a callback */
    struct sigevent *sigev = &ime_d->sigev;
    sigev->sigev_notify = SIGEV_THREAD;
    sigev->sigev_value.sival_ptr = ime_d;
    sigev->sigev_notify_function = aio_end_callback;
    sigev->sigev_notify_attributes = NULL;

    ime_d->aiocb = aiocb;

    /* transform each iovec into an aiocb */
    off_t offset = aiocb->file_offset;
    for (int i = 0; i < aiocb->iovcnt; i++) {
        aiocb_array[i].aio_buf = aiocb->iov[i].iov_base;
        aiocb_array[i].aio_nbytes = aiocb->iov[i].iov_len;
        aiocb_array[i].aio_fildes = aiocb->fd;
        aiocb_array[i].aio_offset = offset;
        aiocb_array[i].aio_lio_opcode = lio_opcode;
        aiocb_array[i].aio_sigevent.sigev_notify = SIGEV_NONE;
        aiocb_array[i].aio_reqprio = 0;

        offset += aiocb_array[i].aio_nbytes;
    }

    /* send the IOs */
    int res = lio_listio(LIO_NOWAIT, ime_d->aiocb_list, aiocb->iovcnt, sigev);
    if (res < 0) {
        free(ime_d->aiocb_list);
        free(ime_d);
        return -1;
    }

    return 0;
}

int ime_native_aio_read(struct ime_aiocb *aiocb)
{
    return aio_op(aiocb, LIO_READ);
}

int ime_native_aio_write(struct ime_aiocb *aiocb)
{
    return aio_op(aiocb, LIO_WRITE);
}

#else
int ime_native_aio_read(struct ime_aiocb *aiocb)
{
    ssize_t res = preadv(aiocb->fd, aiocb->iov, aiocb->iovcnt,
                     aiocb->file_offset);

    int err = 0;
    if (res < 0)
        err = errno;

    aiocb->complete_cb(aiocb, err, res);
    return 0;
}

int ime_native_aio_write(struct ime_aiocb *aiocb)
{
    ssize_t res = pwritev(aiocb->fd, aiocb->iov, aiocb->iovcnt,
                     aiocb->file_offset);

    int err = 0;
    if (res < 0)
        err = errno;

    aiocb->complete_cb(aiocb, err, res);
    return 0;
}
#endif
