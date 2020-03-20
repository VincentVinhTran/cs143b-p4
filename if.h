#ifndef IF_H
#define IF_H


#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "types.h"


void read_block(DISK disk, char* I, int b);

void write_block(DISK disk, char* O, int b);

void init_fs(FS*fs);

int new_file(FS* fs, char* name);

int delete_file(FS* fs, char* name);

int open_file(FS* fs, char* name);

int close_file(FS* fs, int i);

int read_file_to_memory(FS* fs, int i, int m, int n);

int write_memory_to_file(FS* fs, int i, int m, int n);

int do_seek(FS* fs, int i, int p);

int print_dir(FS* fs);

void read_from_memory(FS* fs, int m, int n);

int write_to_memory(FS* fs, int m, char* s);


#endif
