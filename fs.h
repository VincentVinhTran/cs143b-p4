#ifndef FS_H
#define FS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "if.h"


int error();

int init(FS* fs);

int create(FS* fs, char* name);

int destroy(FS* fs, char* name);

int read(FS* fs, int i, int m, int n);

int write(FS* fs, int i, int m, int n);

int open(FS* fs, char* name);

int close(FS* fs, int i);

int seek(FS* fs, int i, int p);

int directory(FS* fs);

int read_memory(FS* fs, int m, int n);

int write_memory(FS* fs, int m, char* s);


#endif
