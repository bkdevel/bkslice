/*
 * SPDX-License Identifier: BSD 0-Clause
 *
 * Copyright (c) 2026 bkdevel
 * All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any 
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT 
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#pragma once


#include <stdbool.h>
#include <stdint.h>


/*
 * Structure of ther bkslice PT Header header
 */
typedef struct
{
    uint16_t magic;             /* Magic number of PT must be 0xFEED */
    uint8_t  disk;              /* Type of disk
                                 * 0 - (P)ATA
                                 * 1 - ATAPI
                                 * 2 - SATA
                                 * 3 - NVME
                                 * 4 - FDD
                                 * 5 - Other
                                 * Other Types will be possibly added in the future
                                 */ 
    char name[110];             /* Name and description of slice, 110 bytes */
    char null;                  /* Must be 0 */
    uint8_t flags;              /* Bit 0:       supports lba?
                                 * Bit 1 - 7:   not used yet
                                 */
    uint16_t sectorSize;        /* Size of one sector in bytes, usually 512 */
    uint16_t sectorsPerTrack;   /* CHS: sectors per Track */
    uint16_t headc;             /* Number of heads */
    uint8_t partc;              /* Number of partitions 1 - 26 */
}__attribute__((packed))
PTHeader;

/*
 * Structure of a bkslice Partition table entry
 */
typedef struct{
    char id;            /* id, char from a to z */
    uint8_t type[2];    /* Type of parition 
                         * NOTE: If you want your OS or FS listed here, pls contact me ie. open an issue or make a PR
                         *  byte 0: OS ID byte
                         *      0x0: None 
                         *      0x1: STULTIX
                         *      0x2 - 0xFF: other
                         *  byte 1: Filesystem ID
                         *      0x00: illegal must be < 0x1
                         *      0x01: UFS1
                         *      0x02: UFS2
                         *      0x03: ZFS
                         *      0x04: btrfs
                         *      0x05: FAT12
                         *      0x06: FAT16
                         *      0x07: FAT32
                         *      0x08: exFAT
                         *      0x09: XFS
                         *      0x0A: APFS
                         *      0x0B: HFS
                         *      0x0C: HFS+
                         *      0x0D: NTFS
                         *      0x0E: LEAN
                         *      0x0F: ReiserFS
                         *      0x10: HPFS
                         *      0x11: Minix 1 FS
                         *      0x12: Minix 2 FS
                         *      0x13: Minix 3 FS
                         *      0x14 - 0x21: other
                         *      0x22: ext2
                         *      0x23: ext3
                         *      0x24: ext4
                         *      0x25 - 0x2F: other
                         *      0x30: vFAT
                         *      0x31: HAMMER
                         *      0x32: HAMMER2
                         *      0x33 - 0x40: other
                         *      0x40: BeFS
                         *      0x41: ZDSFS
                         *      0x42: USTAR
                         *      0x43: ISO 9660
                         *      0x44: Joilet
                         *      0x55: UDF
                         *      0x56 - 0xFE
                         *      0xFF: None
                         */
    uint8_t flags;      /* Bit 0:    is bootable?
                         * Bit 1:    is writeable? 
                         * Bit 2:    is root?
                         * Bit 3:    is rescue?
                         * Bit 4:    is swap?
                         * Bit 5:    is deticated boot partition?
                         * Bit 6 & 7:not used (yet)                        
                         */
    uint32_t size;      /* size of one partition in sectors */
    uint32_t lba;       /* LBA of first sector of partition */
    uint8_t  chs[3];    /* CHS of first sector of partition 
                         * Byte 0: Cylinder
                         * Byte 1: Head
                         * Byte 2: sectot
                         */
}__attribute__((packed))
PTEntry;


/*
 * Structure of the entire bkslice PT 
 */
typedef struct
{
    PTHeader Header;
    PTEntry  Entry[26]; 
}__attribute__((packed))
PTStruct;