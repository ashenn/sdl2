#include <sys/time.h>
#include <SDL2/SDL.h>
#include "common.h"

Class* newClass(size_t s) {
	static int id = 0;
	Class* cl = (Class*) calloc(1, s);

	if (cl == NULL) {
		return NULL;
	}

	cl->id = id++;
	cl->pid = -1;
	cl->name = NULL;
	cl->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	cl->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	return cl;
}

void th_lock(Class* cl) {
	//logger->err(LOG_MAIN, "TEST: %s", cl->name);
	pid_t curTH = getpid();
	//logger->err(LOG_MAIN, "LOCKING: %d", curTH);
	//logger->err(LOG_MAIN, "CURRENT: %d", cl->pid);

	if (curTH == cl->pid) {
		//logger->err(LOG_MAIN, "LOCKING Object IN SAME THREAD");
		//assert(1);
		return;
	}

	cl->pid = curTH;
	//logger->err(LOG_MAIN, "Locked By: %d", cl->pid);
	pthread_mutex_lock(&cl->mutex);
}

void th_unlock(Class* cl) {
	if (cl->pid == getpid())
	{
		cl->pid = -1;
		pthread_mutex_unlock(&cl->mutex);
	}
	else if(cl->pid >= 0){
		logger->err(LOG_MAIN, "Trying To Unlock Thread But Mutex Was Locked By An Other: %s", cl->name);
	}
	else {
		logger->err(LOG_MAIN, "Trying To Unlock Thread None Locked Mutex: %s", cl->name);
	}
}

void th_wait(Class* cl) {
	pthread_cond_wait(&cl->cond, &cl->mutex);
}

void th_wait_time(Class* cl, float delay) {
	struct timespec waitTime;
    struct timeval now;
	int seconds = (int)delay;
    float microsec =  (delay - ((float) seconds)) * 1000.0f;

	gettimeofday(&now,NULL);

    waitTime.tv_sec = now.tv_sec + (int) delay;
    waitTime.tv_nsec = (now.tv_usec + microsec) *1000UL;

	pthread_cond_timedwait(&cl->cond, &cl->mutex, &waitTime);
}

void th_signal(Class* cl) {
	pthread_cond_signal(&cl->cond);
}

int tickWait(int next) {
	int now = SDL_GetTicks();

    if(next <= now){
        return 0;
    }

    int time = next- now;
	SDL_Delay(time);

	return time;
}

float microTime() {
    struct timeval time;
    gettimeofday(&time, NULL);
	unsigned long res = ((unsigned long) time.tv_sec * 1000000) + (unsigned long) time.tv_usec;

    return (float) res;
}
