# IME Client wrapper library

It is used to mock IME native calls using POSIX functions.

## Compilation

To compile with proper AIO support (needs POSIX list IOs):
```
gcc -shared -fpic -DUSE_LIO ime_client.c -o libim_client.so
```

To compile without proper AIO support (fallback to blocking calls):
```
gcc -shared -fpic ime_client.c -o libim_client.so
```