#ifndef __LOGGER_H__
#define __LOGGER_H__

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define DEBUG 0
#define INFO 1
#define WARNING 2
#define ERROR 3

#define LOG_NONE 0
#define LOG_ALL 1

#include "libParse.h"
#include <pthread.h>

#ifdef _WIN32
    #include <sys\stat.h>
    #define LOGGER_FILE_WRITE _S_IRUSR|_S_IWUSR
#else
    #define LOGGER_FILE_WRITE S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP
#endif // _WIN32

typedef struct Log{
	short lvl;	// Output min lvl
	short isInit;
	short enabled;

	char* file;	// LogFile Path
	int f;

	void* init;
	void* log;

	void (*dbg)(unsigned int tag, char* msg, ...);
	void (*inf)(unsigned int tag, char* msg, ...);
	void (*err)(unsigned int tag, char* msg, ...);
	void (*war)(unsigned int tag, char* msg, ...);
	void (*close)();

	char** lvls; // Existing debug lvl
	char** lvlColors; // Existing debug lvl colors

	ListManager* args;
	ListManager* tags;

	pthread_cond_t cond;
	pthread_mutex_t mutex;
} Log;


Log* logger;

Log* getLogger();
Log* initLogger(int argc, char *argv[]);
void closeLogger();
void setLogLvl(int lvl);
short setLogFile(Log* logger, char* file);
void logg(short lvl, unsigned int tag, char* msg, va_list* args);

void addLoggerTag(unsigned int  tag, char* name, short active);
short enableLoggerTag(unsigned int  tag);
short disableLoggerTag(unsigned int  tag);

#endif
