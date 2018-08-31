#include <SDL2/SDL.h>
#include "common.h"

Class* newClass(size_t s) {
	static int id = 0;
	Class* cl = (Class*) malloc(s);

	if (cl == NULL) {
		logger->err(LOG_COMMON, "FAIL TO MALLOC CLASS OF SIZE: %d", (int) s);
		return NULL;
	}

	cl->id = id++;
	cl->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	cl->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	return cl;
}

char* Str(const char* str) {
    int len = strlen(str)+1;
    char* res =  malloc(len);

    memset(res, 0, len);
    strcpy(res, str);

    return res;
}

char* StrE(const int len) {
    char* res =  malloc(len);
    memset(res, 0, len);

    return res;
}

void th_lock(Class* cl) {
	pthread_mutex_lock(&cl->mutex);
}

void th_unlock(Class* cl) {
	pthread_mutex_unlock(&cl->mutex);
}

void th_wait(Class* cl) {
	pthread_cond_wait(&cl->cond, &cl->mutex);
}

void th_signal(Class* cl) {
	pthread_cond_signal(&cl->cond);
}

void tickWait(int next) {
	int now = SDL_GetTicks();

    if(next <= now){
        return;
    }

	SDL_Delay(next - now);
}
