#include <sys/time.h>
#include <SDL2/SDL.h>
#include "common.h"

Class* newClass(size_t s) {
	static int id = 0;
	Class* cl = (Class*) malloc(s);

	if (cl == NULL) {
		return NULL;
	}

	cl->id = id++;
	cl->pid = -1;
	cl->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	cl->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	return cl;
}

void th_lock(Class* cl) {
	pid_t curTH = getpid();
	//logger->err(LOG_MAIN, "LOCKING: %d", curTH);
	//logger->err(LOG_MAIN, "CURRENT: %d", cl->pid);
	
	if (curTH == cl->pid) {
		logger->err(LOG_MAIN, "LOCKING Object IN SAME THREAD");
		assert(1);
	}

	pthread_mutex_lock(&cl->mutex);
}

void th_unlock(Class* cl) {
	cl->pid = -1;
	pthread_mutex_unlock(&cl->mutex);
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
