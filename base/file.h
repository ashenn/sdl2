#ifndef FILE_H
#define FILE_H

char* fileGetContent(char* path);
int filePutContent(char* path, const char* data, short append);

#endif