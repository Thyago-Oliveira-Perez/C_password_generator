## Description

A small terminal password generator written in C, originally built to study basic
C programming concepts. It lets you pick a password length and which character
types to include (uppercase, lowercase, digits, symbols), guarantees at least one
character from each selected type, and shuffles the result.

Random bytes come from `/dev/urandom` on Linux/macOS/BSD, falling back to a
time-seeded `rand()` on other platforms (e.g. Windows).

## Building

Requires a C compiler (`gcc` or compatible) and `make`. Works on Linux, macOS,
and Windows (MinGW) — no OS-specific headers are used.

```bash
make
```

This produces the `password_generator` (or `password_generator.exe` on Windows)
executable. Alternatively, compile directly:

```bash
gcc -Wall -Wextra -std=c11 main.c functions.c -o password_generator
```

## Running

```bash
./password_generator
# or
make run
```

## Cleaning build artifacts

```bash
make clean
```
