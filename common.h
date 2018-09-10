#ifndef COMMON_H
#define COMMON_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

#include "base/logger.h"

#define CLASS_BODY \
	int id;	\
	char* name;	\
	\
	pthread_cond_t cond;	 \
	pthread_mutex_t mutex;

typedef	struct Class
{
	CLASS_BODY
} Class;

typedef enum bool
{
	false,
	true
} bool;

#define SCREEN_W 800
#define SCREEN_H 600

#define FPS 60

#define LOG_MAIN 2
#define LOG_OBJ 4
#define LOG_ASSET 8
#define LOG_VIEW 16
#define LOG_LAYER 21
#define LOG_EVENT 64
#define LOG_COMMON 128
#define LOG_PROJECT 256
#define LOG_RENDER 512
#define LOG_ANIM 1024
#define LOG_TIMER 2048
#define LOG_SPRITE 4096


Class* newClass(size_t s);
#define new(T) ((T*) newClass(sizeof(T)))


char* Str(const char* str);
char* StrE(const int len);


void th_lock(Class* cl);
void th_wait(Class* cl);
void th_unlock(Class* cl);
void th_signal(Class* cl);
void th_wait_time(Class* cl, float delay);

#define LOCK(T) (th_lock((Class*) T))
#define UNLOCK(T) (th_unlock((Class*) T))

#define WAIT(T) (th_wait((Class*) T))
#define SIGNAL(T) (th_signal((Class*) T))
#define WAIT_TIME(T, D) (th_wait_time((Class*) T, D));

int tickWait(int next);

#endif
