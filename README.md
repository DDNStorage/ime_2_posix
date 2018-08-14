# IME Client wrapper library

It is used to mock IME native calls using POSIX functions.

## Compilation

To compile with proper AIO support (needs POSIX list IOs):
```
make lio
```

To compile without proper AIO support (fallback to blocking calls):
```
make nolio
```
