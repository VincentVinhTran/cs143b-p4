#include "if.h"


int valid_disk_block(int b) {
	return b >= 0 && b < DISKSIZE;
}


int valid_fd(int i) {
	return i >= 0 && i < NUMFD;
}


int valid_oft_entry(int i) {
	return i >= 0 && i < OFTSIZE;
}


int valid_of_pos(int pos) {
	return pos >= 0 && pos < BLOCKSIZE*3;
}




void copy_block(char* dst, char* src, int size) {
	for(int i = 0; i < size; ++i) {
		dst[i] =  src[i];
	}	
}


void read_block(DISK disk, char* I, int b) {
	copy_block(I, disk[b], BLOCKSIZE);
}


void write_block(DISK disk, char* O, int b) {
	copy_block(disk[b], O, BLOCKSIZE);
}


void alloc_block(FS* fs, int b) {
	fs->bm[b] = TRUE;
}


void dealloc_block(FS* fs, int b) {
	fs->bm[b] = FALSE;
}


int new_block(FS* fs) {
	for(int i = 0; i < BLOCKSIZE; ++i) {
		if(fs->bm[i] == FALSE) {
			alloc_block(fs, i);
			return i;
		}
	}

	return ERROR;
}


void free_block(FS* fs, int b) {
	dealloc_block(fs, b);
	memset(fs->disk[b], 0, BLOCKSIZE);
}




FD* get_fd(FS* fs, int i) {
	return &fs->fds[i];
}


void set_fd(FS* fs, int i, int size, int b0, int b1, int b2) {
	FD* fd = get_fd(fs, i);

	fd->size = size;
	fd->blocks[0] =  b0;
	fd->blocks[1] =  b1;
	fd->blocks[2] =  b2;
}


int new_fd(FS* fs) {
	for(int i = 0; i < NUMFD; ++i) {
		if(fs->fds[i].size == FREE) {
			fs->fds[i].size = 0;
			return i;
		}
	}

	return ERROR;
}


void free_fd(FS* fs, int i) {
	FD* fd = get_fd(fs, i);

	fd->size = FREE;
	
	for(int i = 0; i < NUMFDBLOCK; ++i) {
		if(fd->blocks[i] != FREE) {
			free_block(fs, fd->blocks[i]);
			fd->blocks[i] = FREE;
		}
	}
}


int assign_block(FS* fs, int f, int b) {
	int* blocks = get_fd(fs, f)->blocks;

	for(int i = 0; i < NUMFDBLOCK; ++i) {
		if(blocks[i] == FREE) {
			blocks[i] = b;
			return i;
		}
	} 

	return ERROR;
}


int block_index(int pos) {
	return pos/BLOCKSIZE;
}


int map_pos_to_block(int pos) {
	return pos % BLOCKSIZE;
}




OF* get_of(FS* fs, int i) {
	if(fs->oft[i].pos != FREE) {
		return &fs->oft[i];
	}

	return NULL;
}


void set_of(FS* fs, int i, int pos, int size, int fd) {
	fs->oft[i].pos = pos;
	fs->oft[i].size = size;
	fs->oft[i].fd = fd;
}


int new_of(FS* fs) {
	for(int i = 0; i < OFTSIZE; ++i) {
		if(fs->oft[i].pos == FREE) {
			fs->oft[i].pos = 0;
			return i;
		}
	}

	return ERROR;
}


void free_of(FS* fs, int i) {
	set_of(fs, i, FREE, FREE, FREE);
}


int of_is_open(FS* fs, int i) {
	return fs->oft[i].pos != FREE;
}


int find_of(FS* fs, int fd) {
	for(int i = 0; i < OFTSIZE; ++i) {
		if(fs->oft[i].fd == fd)  {
			return i;
		}
	}

	return ERROR;
}


void move_of_pos(FS* fs, int i, int p) {
	OF* of = get_of(fs, i);

	int old_b = block_index(of->pos);
	int new_b = block_index(p);

	if(old_b != new_b) {
		FD* fd = get_fd(fs, of->fd);
		int old_block = fd->blocks[old_b];
		int new_block = fd->blocks[new_b];
		write_block(fs->disk, of->buf, old_block);
		read_block(fs->disk, of->buf, new_block);
	}

	of->pos = p;
}




int de_is_free(DE* de) {
	return de->name[0] == '\0';
}


void reset_dir(FS* fs) {
	do_seek(fs, DIROF, 0);
}


int next_de(FS* fs) {
	OF* dof = get_of(fs, DIROF);
	
	return do_seek(fs, DIROF, dof->pos+sizeof(DE));	
}


DE* curr_de(FS* fs) {
	OF* dof = get_of(fs, DIROF);
	int p = map_pos_to_block(dof->pos);

	return (DE*)&dof->buf[p];
}


DE* new_de(FS* fs, char* name, int fd) {
	OF* dof = get_of(fs, DIROF);
	int p = map_pos_to_block(dof->size);
	DE* de = (DE*)&dof->buf[p];
	strncpy(de->name, name, sizeof(de->name));
	de->fd = fd;
	
	int old_size = dof->size;
	dof->size += sizeof(DE);
	if(block_index(dof->size) > block_index(old_size)) {
		int b = new_block(fs);
		assign_block(fs, DIRFD, b);
	}

	return de;	
}


void free_de(DE* de) {
	memset(de->name, 0, sizeof(de->name));
	de->fd = FREE;
}


int de_exists(FS* fs, char* name) {
	reset_dir(fs);

	DE* de;
	do {
		de = curr_de(fs);
		if(strncmp(de->name, name, sizeof(de->name)) == 0) {
			return TRUE;
		}
	} while(next_de(fs) == 0);

	return FALSE;
}


int can_open(FS* fs, char* name) {
	if(!de_exists(fs, name)) {
		return FALSE;
	}

	DE* de = curr_de(fs);
	
	for(int i = 0; i < OFTSIZE; ++i) {
		if(fs->oft[i].fd == de->fd) {
			return FALSE;
		}
	}	

	return TRUE;
}




void init_ptrs(FS* fs) {
	fs->bm = fs->disk[0];
	fs->fds = (FD*)&fs->disk[1];
}


void clear_disk(FS* fs) {
	for(int i = 0; i < DISKSIZE; ++i) {
		memset(fs->disk[i], 0, BLOCKSIZE);
	}

	for(int i = 0; i < NUMFD; ++i) {
		free_fd(fs, i);
	}

	for(int i = 0; i < DIRBLOCK; ++i) {
		alloc_block(fs, i);
	}
}


void clear_buffers(FS* fs) {
	memset(fs->I, 0, BLOCKSIZE);
	memset(fs->O, 0, BLOCKSIZE);
	memset(fs->M, 0, BLOCKSIZE);
}


void clear_oft(FS* fs) {
	for(int i = 0; i < OFTSIZE; ++i) {
		memset(fs->oft[i].buf, 0, BLOCKSIZE);
		fs->oft[i].pos = FREE;
		fs->oft[i].size = FREE;
		fs->oft[i].fd = FREE;
	}
}


void init_dir(FS* fs) {
	fs->fds[0].size = 0;
	alloc_block(fs, DIRBLOCK);
	assign_block(fs, DIRFD, DIRBLOCK);
	fs->oft[0].pos = 0;
	fs->oft[0].size = 0;
	fs->oft[0].fd = DIRFD;
}


void init_fs(FS* fs) {
	init_ptrs(fs);
	clear_disk(fs);
	clear_buffers(fs);
	clear_oft(fs);
	init_dir(fs);
}




int new_file(FS* fs, char* name) {
	int len = strlen(name);
	if(len < 1 || len > 3) {
		return ERROR;
	}

	if(de_exists(fs, name)) {
		return ERROR;
	}

	int f = new_fd(fs);
	if(f == ERROR) {
		return ERROR;
	}

	new_de(fs, name, f);

	return 0;
}




int delete_file(FS* fs, char* name) {
	if(!de_exists(fs, name)) {
		return ERROR;
	}

	DE* de = curr_de(fs);

	free_fd(fs, de->fd);

	free_de(de);				

	return 0;
}




int open_file(FS* fs, char* name) {
	if(!can_open(fs, name)) {
		return ERROR;
	}

	int o = new_of(fs);
	if(o == ERROR) {
		return ERROR;
	}

	DE* de = curr_de(fs);
	FD* fd = get_fd(fs, de->fd);
	OF* of = &fs->oft[o];
	
	of->size = fd->size;
	of->fd = de->fd;
	
	if(of->size == 0) {
		int b = new_block(fs);
		if(b == ERROR) {
			return ERROR;
		} 
		assign_block(fs, of->fd, b);
	} else {
		read_block(fs->disk, of->buf, fd->blocks[0]);
	}

	return o;
}




int close_file(FS* fs, int i) {
	if(!valid_oft_entry(i) || i == DIROF || !of_is_open(fs, i)) {
		return ERROR;
	}

	OF* of = get_of(fs, i);
	FD* fd = get_fd(fs, of->fd);

	int b = block_index(of->pos);
	int block = fd->blocks[b];

	write_block(fs->disk, of->buf, block);
	fd->size = of->size;
	free_of(fs, i);		

	return 0;
}	




int read_file_to_memory(FS* fs, int i, int m, int n) {
	if(!valid_oft_entry(i) || !of_is_open(fs, i)) {
		return ERROR;	
	}

	OF* of = get_of(fs, i);
	int j = 0;
	int k = m;
	int p;

	while(of->pos < of->size && j < n) {
		p = map_pos_to_block(of->pos);
		fs->M[k] = of->buf[p];
	
		++j;
		++k;
		
		if(do_seek(fs, i, of->pos+1) != 0) {
			break;
		}
	}

	return j;
}




int write_memory_to_file(FS* fs, int i, int m, int n) {
	if(!valid_oft_entry(i) || !of_is_open(fs, i)) {
		return ERROR;
	}

	OF* of = get_of(fs, i);
	int j = 0;
	int k = m;
	int p;
	while(of->pos < BLOCKSIZE*3 && j < n) {
		p = map_pos_to_block(of->pos);
		of->buf[p] = fs->M[k];

		++j;
		++k;
		
		int old_b = block_index(of->pos);
		int new_b = block_index(of->pos+1);
		if(of->pos+1 < BLOCKSIZE*3 && j < n && old_b != new_b) {
			int b = new_block(fs);
			if(b == ERROR) {
				return ERROR;
			}

			if(assign_block(fs, of->fd, b) == ERROR) {
				return ERROR;
			}
		}

		move_of_pos(fs, i, of->pos+1);
	} 

	if(of->pos > of->size) {
		of->size = of->pos;
		get_fd(fs, of->fd)->size = of->size;
	}

	return j;
}




int do_seek(FS* fs, int i, int p) {
	OF* of = get_of(fs, i);

	if( p > of->size 
		|| !valid_oft_entry(i)
		|| !valid_of_pos(p)) {
		return ERROR;		
	}

	move_of_pos(fs, i, p);

	return 0;
}




int print_dir(FS* fs) {
	reset_dir(fs);

	DE* de;
	FD* fd;
	do {
		de = curr_de(fs);
		if(!de_is_free(de)) {
			fd = get_fd(fs, de->fd);
			printf("%s %d ", de->name, fd->size);
		}
	}
	while(next_de(fs) == 0);
	printf("\n");
	
	return 0;
}




void read_from_memory(FS* fs, int m, int n) {
	copy_block(fs->O, (char*)&fs->M[m], n);
	fs->O[n] = '\0';
}	




int write_to_memory(FS* fs, int m, char* s) {
	int i, j;
	for(i = 0, j = m; s[i] != '\0'; ++i, ++j) {
		fs->M[j] = s[i];		
	}
	
	return i;
}
