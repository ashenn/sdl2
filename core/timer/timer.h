#ifndef TIMER_H
#define TIMER_H

#include <stdarg.h>
#include "../../common.h"

typedef struct DelayedFunction DelayedFunction;
typedef struct DelayedFncLauncher DelayedFncLauncher;

struct DelayedFncLauncher {
	int id;
	Node* node;
	Node* fnc_node;
	
	va_list args;


	pthread_t thread;
	pthread_t fnc_thread;
};

struct DelayedFunction {
	bool loop;
	char* name;
	double delay;

	Node* node;
	Node* timer_node;
	
	va_list args;
	pthread_t thread;
	pthread_t timer_thread;
	
	void* (*callback)();
	void* (*fnc)(void*);
};

void clearDelayedFunctions();
void delayed(double delay, bool loop, void* (*fnc)(void* arg), void* (*callback)(), ...);

#endif
