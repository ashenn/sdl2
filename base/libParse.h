#ifndef __LIBPARSE_H__
#define __LIBPARSE_H__

#include "libList.h"

typedef struct Arg{
	char* name;
	short hasParam;
	void* defParam;
	short asInt;
	char* type;
	void (*function)();
} Arg;

int parseArgs(ListManager* lstMgr, int argc, char* argv[]);
int isArgument(char* str);

ListManager* defineArgs(Arg** params);
ListManager* initLibParse();
int callArg(ListManager* lstMgr, char* name, void* val);

#endif
