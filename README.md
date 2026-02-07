# bkslice

A simple partition table designed for hobby operating system projects.

## Overview

bkslice is the reference implementation of the bkslice-PT.

For info about options: 
```sh 
bkslice --help
```

## Building & Installing

### Dependencies
- A POSIX System (eg. *BSD, Linux, macOS X), if you are on Windows try using WSL.

The project uses a standard `makefile` for building (tested with bmake and gmake):

```sh
make
```

For installing: 

as root:
```sh
make install
```

## License

bkslice is dual-licensed:
- **all files except parttable.h** are licensed under the BSD 3-Clause License
- **parttable.h** is licensed under the BSD 0-Clause License (public domain equivalent)

## Author

Copyright (c) 2026 bkdevel

## The PT

If you want your OS or FS to have its own type in the PT, open an issue or make a PR.

The PT is exactly 512 bytes big and is structured as following (take a look at parttable.h for details):

| Offset | Size   | Content                   | Description                           |
|--------|--------|---------------------------|---------------------------------------|
| 0x0    | 2B     | PTHeader.magic            | Magic number (0xFEED)                 |
| 0x2    | 1B     | PTHeader.disk             | Type of disk (see parttable.h)        |
| 0x3    | 110B   | PTHeader.name             | Name & description of slice/disk      |
| 0x71   | 1B     | PTHeader.null             | Must be 0                             |
| 0x72   | 1B     | PTHeader.flags            | Flags, see below for more information |
| 0x73   | 2B     | PTHeader.sectorSize       | Size of one sector in bytes           |
| 0x75   | 2B     | PTHeader.sectorsPerTrack  | Number of sectors per Track (CHS)     |
| 0x77   | 2B     | PTHeader.headc            | Number of heads (CHS)                 |
| 0x79   | 1B     | PTHeader.partc            | Number of partitions                   |
| 0x7A   | 15B*26 | PTEntry[26]               | Partition Entry                       |

An parition entry looks like that:

| Offset | Size | Content                   | Description                           |
|--------|------|---------------------------|---------------------------------------|
| 0x0    | 1B   | PTEntry.id                | lowercase letter as id of the parition|
| 0x1    | 1B   | PTEntry.type[0]           | OS-ID (see parttable.h)               |
| 0x2    | 1B   | PTEntry.type[1]           | FS-ID (see parttable.h)               |
| 0x3    | 1B   | PTEntry.flags             | Flags, see below for more information |
| 0x4    | 4B   | PTEntry.size              | Size of partition in sectors          |
| 0x8    | 4B   | PTEntry.lba               | LBA of first sector                   |
| 0xC    | 3B   | PTEntry.chs               | CHS of first sector                   |

Header flags (PTHeader.flags):

| Bit | Content                                |
|-----|----------------------------------------|
| 0   | Set, if the sector adressing is in LBA |
| 1-7 | Not used yet                           |

Entry flags (PTEntry.flags):

| Bit | Content                                                          |
|-----|------------------------------------------------------------------|
| 0   | Set, if the partition is bootable                                |
| 1   | Set, if the partition is writeable                               |
| 2   | Set, if the partition is mountable as root                       |
| 3   | Set, if the partition is a dedicated rescue partition (only one) |
| 4   | Set, if the partition is a swap                                  |
| 5   | Set, if the partiition is a dedicated boot partition (only one)  |
| 6&7 | Not used yet                                                     |
