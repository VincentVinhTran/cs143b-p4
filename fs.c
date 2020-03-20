#include "fs.h"


int error() {
	printf("error\n");
	return ERROR;
}


int init(FS* fs) {
	init_fs(fs);
	printf("system initialized\n");
	return 0;
}


int create(FS* fs, char* name) {
	if(new_file(fs, name) != 0) {
		return error();
	}
	printf("%s created\n", name);
	return 0;
}


int destroy(FS* fs, char* name) {
	if(delete_file(fs, name) != 0) {
		return error();
	}
	printf("%s destroyed\n", name);
	return 0;
}


int open(FS* fs, char* name) {
	int o = open_file(fs, name);
	if(o == ERROR) {
		return error();
	}
	printf("%s opened %d\n", name, o);
	return 0;
}


int close(FS* fs, int i) {
	if(close_file(fs, i) != 0) {
		return error();
	}
	printf("%d closed\n", i);
	return 0;
}


int read(FS* fs, int i, int m, int n) {
	int r = read_file_to_memory(fs, i, m, n); 
	if(r < 0) {
		return error();
	}
	printf("%d bytes read from %d\n", r, i);
	return 0;
}


int write(FS* fs, int i, int m, int n) {
	int w = write_memory_to_file(fs, i, m, n);
	if(w < 0) {
		return error();
	}
	printf("%d bytes written to %d\n", w, i);
	return 0;
}


int seek(FS* fs, int i, int p) {
	if(do_seek(fs, i, p) != 0) {
		return error();
	} 
	printf("position is %d\n", p);
	return 0;
}


int directory(FS* fs) {	
	if(print_dir(fs) != 0) {
		return error();
	}
	return 0;
}


int read_memory(FS* fs, int m, int n) {
	read_from_memory(fs, m, n);
	printf("%s\n", fs->O);
	return 0;
}


int write_memory(FS* fs, int m, char* s) {
	int n = write_to_memory(fs, m, s);
	printf("%d bytes written to M\n", n);
	return 0;
}



