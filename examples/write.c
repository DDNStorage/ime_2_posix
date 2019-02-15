#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ime_native.h>

static void my_exit(const char *msg, int ret)
{
    if (ret)
        fprintf(stderr, "%s : err=%s", msg, strerror(ret));
    ime_native_finalize();
    exit(ret);
}

#define FILENAME "ime://testfile"
#define NUM_INTEGERS 255
#define IO_LENGTH (NUM_INTEGERS * sizeof(int))

int main(void)
{
    int fd;
    int buf[NUM_INTEGERS];
    ssize_t nbytes = 0, rc;

    /* Initialize the IME native client.
     */
    ime_native_init();

    /* Open a file via IME.
     */
    fd = ime_native_open(FILENAME, O_RDWR | O_CREAT, 0666);
    if (fd < 0)
        my_exit("ime_native_open() failed", errno);

    /* Write a small amount of data to IME.
     */
    while (nbytes < IO_LENGTH)
    {
        rc = ime_native_write(fd, (const char *)buf, IO_LENGTH);
        if (rc < 0)
            my_exit("ime_native_write() failed", errno);
        nbytes += rc;
    }

    /* Synchronize data to IME service.
     */
    if (ime_native_fsync(fd))
        my_exit("ime_native_fsync() failed", errno);

    /* Close the file in IME.
     */
    if (ime_native_close(fd))
        my_exit("ime_native_close() failed", errno);

    my_exit("done", 0);
}

