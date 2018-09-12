#include "../common.h"
#include "./basic.h"
#include "./file.h"

char* fileGetContent(char* path) {
	long length;
	char* buffer = NULL;

	validatePath(path);
	FILE * f = fopen(path, "rb");


	if (!f) {
		return NULL;
	}

	fseek (f, 0, SEEK_END);
	length = ftell(f);
	fseek (f, 0, SEEK_SET);
	buffer = malloc(length);

	if (buffer) {
		fread(buffer, 1, length, f);
	}

	fclose (f);
	return buffer;
}
