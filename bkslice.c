/*
 * SPDX-License Identifier: BSD 3-Clause
 *
 * Copyright (c) 2026 bkdevel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of bkdevel nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#define _POSIX_C_SOURCE 200809L


#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "common.h"
#include "parttable.h"


#define ATA     0
#define ATAPI   1
#define SATA    2
#define NVME    3
#define FDD     4
#define OTHER   5


const char* 
disk[6] = 
{
    [ATA]   = "ata",
    [ATAPI] = "atapi",
    [SATA]  = "sata",
    [NVME]  = "nvme",
    [FDD]   = "fdd",
    [OTHER] = "other"
};


PTStruct g_PT = {0};


void
usage(char** argv)
{

    fprintf(stderr, "Usage: %s [FILE] [-htSsHnolbayra] [OPTION]\n", argv[0]);
    fprintf(stderr, "\t--help             -h\t\t\t Print this usage info\n");
    fprintf(stderr, "\t--type             -t [OPTION]\t\t Set type of disk\n");
    fprintf(stderr, "\t--sector-size      -s [SIZE]\t\t Set size of one sector\n");
    fprintf(stderr, "\t--sector-per-track -S [SIZE]\t\t Set the number of sectors per track\n");
    fprintf(stderr, "\t--heads            -H [NUM]\t\t Set the number of heads\n");
    fprintf(stderr, "\t                   -nolba\t\t Specify that LBA is not supported\n");
    fprintf(stderr, "\t                   -y\t\t\t Don't ask y/n\n");
    fprintf(stderr, "\t--remove-entry     -r [abcde...]\tRemove an entry from the PT\n");
    fprintf(stderr, "\t--add-entry        -a [abcde...][OPTION] Add an entry to the PT\n");
    fprintf(stderr, "\t                                type [OS] [FS]  specify type, note hex value expected see parttable.h for resource\n");
    fprintf(stderr, "\t                                bootable\tSet bootable flag\n");
    fprintf(stderr, "\t                                readonly\tSet the readonly flag\n");
    fprintf(stderr, "\t                                root\t\tSpecify partition as /\n");
    fprintf(stderr, "\t                                rescue\t\tSpecify partition as /rescue\n");
    fprintf(stderr, "\t                                swap\t\tSpecify paritition as swap\n");
    fprintf(stderr, "\t                                boot\t\tSpecify partition as /boot\n");

    exit(1);

}


PTStruct
read_pt(const char* filename){

    if(!filename){
        fprintf(stderr, "ERROR\tfilename not specified");
        exit(1);
    }
    
    int fd = open(filename, O_RDONLY);
    if(fd < 0){
        perror("open");
        exit(1);
    }

    PTStruct i_PT;

    ssize_t n = pread(fd, &i_PT, sizeof(i_PT), 0);
    close(fd);

    if(n != sizeof(i_PT)){
        fprintf(stderr, "ERROR\tOnly %zd bytes read", n);
        exit(1);
    }

    return i_PT;

}


void
write_pt(const char* filename)
{
   
    if(!filename){
        fprintf(stderr, "ERROR\tfilename not specified");
        exit(1);
    }

    int fd = open(filename, O_WRONLY | O_CREAT, 644);
    if(fd < 0){
        perror("open");
        exit(1);
    }

    g_PT.Header.magic = 0xFEED;

    ssize_t n = pwrite(fd, &g_PT, sizeof(g_PT), 0);
    close(fd);

    if(n != sizeof(g_PT)){
        fprintf(stderr, "ERROR\tOnly %zd bytes read", n);
        exit(1);
    }

}


int
add_entry(char** argv, int argc, int i){

    if(strlen(argv[i]) != 1 || !isalpha((unsigned char)argv[i][0])){ 
        fprintf(stderr, "ERROR\tPartition-ID must be a single letter\n");
        exit(1);
    }

    char id = argv[i][0];
    uint8_t partc = 1, part = 0;
    bool isFree[26] = {false};
    for(uint8_t x = 0; x < 26; x++){
        if(g_PT.Entry[x].id == id){
            fprintf(stderr, "ERROR\tPartition %c already exists", id);
            exit(1);
        }
        if(g_PT.Entry[x].type[1]) partc++;
        else isFree[x] = true;
    }

    if(partc == 26){
        fprintf(stderr, "ERROR\tNo Partition entry free");
    }

    for(uint8_t x = 0; x < 26; x++){
        if(isFree[x]){
            part = x;
            break;
        }
    }

    g_PT.Entry[part].flags |= (1u << 1);

    if(i + 1 >= argc) usage(argv);
    i++;
    while(i < argc){
        if(!strcmp(argv[i], "type")){
            if(i + 2 >= argc) usage(argv);
            i++;
            g_PT.Entry[part].type[0] = strtol(argv[i], NULL, 16);
            i++;
            g_PT.Entry[part].type[1] = strtol(argv[i], NULL, 16);
        }
        else if(!strcmp(argv[i], "bootable")) g_PT.Entry[part].flags |= (1u << 0);
        else if(!strcmp(argv[i], "readonly")) g_PT.Entry[part].flags &= ~(1u << 1);
        else if(!strcmp(argv[i], "root")) g_PT.Entry[part].flags |= (1u << 2);
        else if(!strcmp(argv[i], "rescue")) g_PT.Entry[part].flags |= (1u << 3);
        else if(!strcmp(argv[i], "swap")) g_PT.Entry[part].flags |= (1u << 4);
        else if(!strcmp(argv[i], "boot")) g_PT.Entry[part].flags |= (1u << 5);
        else if(!strcmp(argv[i], "size")){
            if(i + 1 >= argc) usage(argv);
            i++;
            g_PT.Entry[part].size = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "lba")){
            if(i + 1 >= argc) usage(argv);
            i++;
            g_PT.Entry[part].lba = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "chs")){
            if(i + 3 >= argc) usage(argv);
            i++;
            for(uint8_t x = 0; x < 3; x++){
                g_PT.Entry[part].chs[x] = atoi(argv[i + x]);
            }
        }


        i++;
    }

    g_PT.Entry[part].id = id;

    g_PT.Header.partc = partc;

    return i;

}


void
remove_entry(char* str)
{

    if(strlen(str) != 1 || !isalpha((unsigned char)str[0])){ 
        fprintf(stderr, "ERROR\tPartition-ID must be a single letter\n");
        exit(1);
    }
    char id = str[0];
    
    uint8_t part = 0;
    bool found = false;
    for(uint8_t x = 0; x < 26; x++){
        if(g_PT.Entry[x].id == id){
            part = x;
            found = true;
            break;
        }
    }

    if(!found){
        printf("Partition %c doesn't exist - Nothing to do\n", id);
        return;
    }

    memset(&g_PT.Entry[part], 0, sizeof(g_PT.Entry[part]));

}


int
main(int argc, char** argv)
{

    if(argc < 2) usage(argv);

    if(argc < 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) usage(argv);
    
    char* filename = argv[1];
    bool yes = false;

    g_PT = read_pt(filename);

    for(int i = 2; i < argc; i++){
        if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--type")){
            if(i + 1 >= argc) usage(argv);
            i++;
            for(uint8_t x = 0; x < 5; x++){
                if(!strcmp(argv[i], disk[x])){
                    g_PT.Header.disk = x;
                    break;
                }
            }
        }
        else if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--name")){
            if(i + 1 >= argc) usage(argv);
            i++;
            if(strlen(argv[i]) > 110){
                fprintf(stderr, "ERROR:\tName must not be longer than 110 bytes");
                return 1;
            }
            for(int x = 0; x <= 110; x++){ 
                if(!argv[i][x]) break;
                g_PT.Header.name[x] = argv[i][x];
            }
        }
        else if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--sector-size")){
            if(i + 1 >= argc) usage(argv);
            i++;
            g_PT.Header.sectorSize = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "-S") || !strcmp(argv[i], "--sector-p-track")){
            if(i + 1 >= argc) usage(argv);
            i++;
            g_PT.Header.sectorsPerTrack = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "-no-lba"))   g_PT.Header.flags = 1;
        else if(!strcmp(argv[i], "-H") || !strcmp(argv[i], "--heads")){  
            if(i + 1 >= argc) usage(argv);
            i++;
            g_PT.Header.headc = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--add-entry")){
            if(i + 1 >= argc) usage(argv);
            i++;
            i = add_entry(argv, argc, i) - 2;
        }
        else if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--remove-entry")){
            if(i + 1 >= argc) usage(argv);
            i++;
            remove_entry(argv[i]);
        }
        else if(!strcmp(argv[i], "-y")) yes = true;
        else{
            fprintf(stderr, "ERROR:\tInvalid Argument %s\n", argv[i]);
            usage(argv);
        }
    }

    if(!yes){
        char y;
        printf("Write [Y/n]: ");
        scanf(" %c", &y);
        if(y == 'y' || y == 'Y') yes = true;
    }
    if(yes) write_pt(filename);

    return 0;

}
