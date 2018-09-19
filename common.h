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
	pthread_mutex_t mutex;	\
	\
	pid_t pid;

typedef	struct Class
{
	CLASS_BODY
} Class;

typedef enum bool
{
	false,
	true
} bool;

#define SCREEN_W 400
#define SCREEN_H 400

#define FPS 60
#define FRAME_DURATION 1000 / FPS

#define LOG_MAIN 		1 << 1
#define LOG_OBJ 		1 << 2
#define LOG_ASSET 		1 << 3
#define LOG_VIEW 		1 << 4
#define LOG_LAYER 		1 << 5
#define LOG_EVENT 		1 << 6
#define LOG_COMMON 		1 << 7
#define LOG_PROJECT 	1 << 8
#define LOG_RENDER 		1 << 9
#define LOG_ANIM 		1 << 10
#define LOG_TIMER 		1 << 11
#define LOG_SPRITE 		1 << 12
#define LOG_CHAR 		1 << 13
#define LOG_CONTROL		1 << 14
#define LOG_CONTROLLER 	1 << 15
#define LOG_MOVE 		1 << 16
#define LOG_COLLISION	1 << 17

#define FLIP_N SDL_FLIP_NONE
#define FLIP_V SDL_FLIP_VERTICAL
#define FLIP_H SDL_FLIP_HORIZONTAL


Class* newClass(size_t s);
#define new(T) ((T*) newClass(sizeof(T)))

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

float microTime();
int tickWait(int next);

#endif
