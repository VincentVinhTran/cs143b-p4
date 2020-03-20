#include "shell.h"


void strip(char* s) {
	int n = strlen(s);
	if(s[n-1] == '\n') {
		s[n-1] = '\0';
	}
}


int match(FS* fs, const char* cmd) {
	return strncmp(fs->I, cmd, 2) == 0;
}


void shell() {
	FS file_system;
	FS* fs = & file_system;

	char cmd[3];
	char s[BUFSIZ];
	char name[4];
	int i, m, n, p;

	while(fgets(fs->I, BLOCKSIZE, stdin)) {
		strip(fs->I);

		if(match(fs, "cr")) {
			sscanf(fs->I, "%s %s", cmd, name);
			strip(name);
			create(fs, name);
		} else if(match(fs, "de")) {
			sscanf(fs->I, "%s %s", cmd, name);
			strip(name);
			destroy(fs, name);
		} else if(match(fs, "op")) {
			sscanf(fs->I, "%s %s", cmd, name);
			strip(name);
			open(fs, name);
		} else if(match(fs, "cl")) {
			sscanf(fs->I, "%s %d", cmd, &i);
			close(fs, i);
		} else if(match(fs, "rd")) {
			sscanf(fs->I, "%s %d %d %d", cmd, &i, &m, &n);
			read(fs, i, m, n);
		} else if(match(fs, "wr")) {
			sscanf(fs->I, "%s %d %d %d", cmd, &i, &m, &n);
			write(fs, i, m, n);
		} else if(match(fs, "sk")) {
			sscanf(fs->I, "%s %d %d", cmd, &i, &p);
			seek(fs, i, p);
		} else if(match(fs, "dr")) {
			directory(fs);
		} else if(match(fs, "in")) {
			init(fs);
		} else if(match(fs, "rm")) {
			sscanf(fs->I, "%s %d %d", cmd, &m, &n);
			read_memory(fs, m, n);
		} else if(match(fs, "wm")) {
			sscanf(fs->I, "%s %d %s", cmd, &m, s);
			write_memory(fs, m, s);
		} else {
			printf("\n");
		}
	}
}
