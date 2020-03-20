#ifndef TYPES_H
#define TYPES_H


#include "defs.h"


typedef char DISK[DISKSIZE][BLOCKSIZE];

typedef struct {
	char buf[512];
	int pos;
	int size;
	int fd;
} OF;

typedef OF OFT[OFTSIZE];

typedef struct {
	int size;
	int blocks[3];
} FD;

typedef struct {
	char name[4];
	int fd;
} DE;

typedef struct {
	DISK disk;
	char I[BLOCKSIZE];
	char O[BLOCKSIZE];
	char M[BLOCKSIZE];
	OFT oft;
	
	char* bm;
	FD* fds;	
} FS;


#endif
