#include "timer.h"
#include <SDL2/SDL.h>
#include "../../base/libList.h"
#include "../project/project.h"

ListManager* getDelayedFunctions() {
	static ListManager* delayedFncs = NULL;

	if (delayedFncs == NULL) {
		delayedFncs = initListMgr();
	}

	return delayedFncs;
}

ListManager* getLaunchers() {
	static ListManager* launchers = NULL;

	if (launchers == NULL) {
		launchers = initListMgr();
	}

	return launchers;
}

void* callDelayedFunction(void* param) {
	logger->inf(LOG_TIMER, "=== CALLING DELAYED FUNCTION ===");

	ListManager* launchers = getLaunchers();
	ListManager* delayedFncs = getDelayedFunctions();
    DelayedFunction* fncParam = (DelayedFunction*) param;


	bool doBreak = 0;
	Project* pro = getProject();

	while (!doBreak && pro->status != PRO_CLOSE) {
		if (fncParam->delay > 0) {
			logger->inf(LOG_TIMER, "-- Waiting: %lf", fncParam->delay);
			SDL_Delay(fncParam->delay * 1000);
		}

		logger->inf(LOG_TIMER, "-- Calling Thread");
		pthread_create(&fncParam->thread, NULL, fncParam->fnc, (void*) fncParam);

		logger->inf(LOG_TIMER, "-- Waiting Thread End");

		pthread_t* th = &(fncParam->thread);
		pthread_join(*th, (void*) &doBreak);


		if (!fncParam->loop || doBreak) {
			logger->inf(LOG_TIMER, "-- Break Timer");
			break;
		}
	}

	logger->inf(LOG_TIMER, "-- Loop Ended");

	logger->inf(LOG_TIMER, "-- Close Args");
	va_end(fncParam->args);

	logger->inf(LOG_TIMER, "-- REMOVE LAUNCHER NODE");
	removeNode(launchers, fncParam->timer_node);

	logger->inf(LOG_TIMER, "-- REMOVE DELAYED NODE");
	removeNode(delayedFncs, fncParam->node);

	logger->inf(LOG_TIMER, "-- FREE PARAM NAME");
	free(fncParam->name);

	logger->inf(LOG_TIMER, "-- FREE PARAM");
	free(fncParam);

	logger->inf(LOG_TIMER, "-- Delayed Done !!!!");
	return NULL;
}

int delayed(double delay, bool loop, void* (*fnc)(void*), void* (*callback)(), ...) {
	static int i = 0;
	logger->inf(LOG_TIMER, "=== NEW DELAYED FUNCTION ===");

	ListManager* delayedFncs = getDelayedFunctions();



	char name[150];
	memset(name, 0, 150);
	snprintf(name, 150, "delayedFnc-%d", i++);
	logger->inf(LOG_TIMER, "-- New Thread: %s", name);


	logger->inf(LOG_TIMER, "-- Prepare Arguments");
	va_list args;
	va_start(args, callback);

	logger->inf(LOG_TIMER, "-- Prepare Structure");
	DelayedFunction* fncParam = malloc(sizeof(DelayedFunction));

	fncParam->fnc = fnc;
	fncParam->loop = loop;
	fncParam->args = args;
	fncParam->delay = delay;
	fncParam->name = Str(name);
	fncParam->callback = callback;

	//va_end(va);

	Node* n = addNodeV(delayedFncs, name, fncParam, 1);
	logger->inf(LOG_TIMER, "-- New Thread ID: %d", n->id);
	fncParam->node = n;

	pthread_t* tmpThread = &(fncParam->timer_thread);

	logger->inf(LOG_TIMER, "-- STORING LAUNCHER");
	ListManager* launchers = getLaunchers();
	fncParam->timer_node = addNodeV(launchers, "Timer Thread", tmpThread, 0);

	logger->inf(LOG_TIMER, "-- Create Call Thread");
	pthread_create(tmpThread, NULL, callDelayedFunction, (void*) fncParam);
}

short clearDelayed(int i, Node* n, short* delete, void* param, va_list* args) {
	logger->inf(LOG_TIMER, "=== WAITING FOR DELAYED: %s ===", n->name);

	pthread_t* th = n->value;
	pthread_join(*th, NULL);

	return true;
}

void clearDelayedFunctions() {
	logger->inf(LOG_TIMER, "##### CLEARING DELAYED FUNCTIONS #####");
	ListManager* launchers = getLaunchers();

	if (launchers->nodeCount) {
		listIterateFnc(launchers, clearDelayed, NULL, NULL);
	}

	logger->inf(LOG_TIMER, "##### DELETING TIMER #####");

	deleteList(launchers);
	deleteList(getDelayedFunctions());
}
