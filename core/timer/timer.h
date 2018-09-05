#ifndef TIMER_H
#define TIMER_H

#include <stdarg.h>
#include "../../common.h"

typedef struct DelayedFunction DelayedFunction;
typedef struct DelayedFncLauncher DelayedFncLauncher;

struct DelayedFncLauncher {
	CLASS_BODY
	Node* node;
	Node* fnc_node;
	
	//va_list args;
	bool killed;
	bool paused;
	bool completed;
	DelayedFunction* fnc;

	pthread_t thread;
	pthread_t* fnc_thread;
};

struct DelayedFunction {
	CLASS_BODY
	double delay;

	bool loop;
	bool killed;
	bool doBreak;
	bool completed;

	Node* node;
	Node* timer_node;

	DelayedFncLauncher* launcher;
	
	va_list args;
	pthread_t thread;
	pthread_t* timer_thread;
	
	void* (*callback)();
	void* (*fnc)(void*);
};

void clearDelayedFunctions();
bool killDelayedFunction(DelayedFncLauncher* launcher, bool force, bool join);

DelayedFncLauncher* delayed(double delay, bool loop, void* (*fnc)(void*), void* (*callback)(DelayedFunction*), ...);

bool pauseDelayedFunction(DelayedFncLauncher* launcher);
bool resumeDelayedFunction(DelayedFncLauncher* launcher);

#endif