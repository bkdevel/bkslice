# bkslice

A simple partition table designed for hobby operating system projects.

## Overview

bkslice is the reference implementation of the bkslice-PT.

## Building & Installing

### Dependencies
- A POSIX System (eg. *BSD, Linux, macOS X), if you are on Windows try using WSL.

The project uses a standard `makefile` for building (tested with bmake and gmake):

```sh
make
```

For installing: 

```sh as root
make install
```

## License

bkslice is dual-licensed:
- **all files except parttable.h** are licensed under the BSD 3-Clause License
- **parttable.h** is licensed under the BSD 0-Clause License (public domain equivalent)

## Author

Copyright (c) 2026 bkdevel

## The PT

To get to know how the PT is structured, please take a look at parttable.h .

If you want your OS or FS to have its own type in the PT, open an issue or make a PR.