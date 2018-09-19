#include "../common.h"
#include "./basic.h"
#include "./file.h"

int filePutContent(char* path, const char* data, short append) {
	//fprintf(stderr, "VALIDATE: %s\n", path);
	validatePath(path);

	//fprintf(stderr, "Opening File: %s\n", path);

	FILE * f = NULL;
	if (append) {
		f = fopen(path, "a");
	}
	else {
		f = fopen(path, "w");
	}

	if (f == NULL) {
		//fprintf(stderr, "Faiil To Open File: %s\n", path);
		return -1;
	}

	//fprintf(stderr, "Printing To File\n");
	//fprintf(stderr, "Data: %s\n", data);
	int total = fprintf(f, data);

	fclose(f);
	return total;
}

char* fileGetContent(char* path) {
	long length;
	char* buffer = NULL;

	//fprintf(stderr, "VALIDATE: %s\n", path);
	validatePath(path);

	//fprintf(stderr, "Opening File: %s\n", path);
	FILE * f = fopen(path, "rb");


	if (!f) {
		return NULL;
	}

	fseek (f, 0, SEEK_END);
	length = ftell(f);
	//fprintf(stderr, "LENGTH: %ld\n", length);
	fseek (f, 0, SEEK_SET);
	buffer = malloc(length);

	if (buffer != NULL) {
		int l = (int) fread(buffer, 1, length, f);
		//fprintf(stderr, "READED: %d\n", l);
		buffer[l] = '\0';
		//fprintf(stderr, "Content:\n %s\n", buffer);
	}

	//fprintf(stderr, "CLOSING FILE\n");
	fclose (f);
	//fprintf(stderr, "FILE CLOSED\n");
	return buffer;
}
