#include "timer.h"
#include <SDL2/SDL.h>

#include "../../base/basic.h"
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

static void cleanLauncher(void* arg) {
	logger->inf(LOG_TIMER, "==== Cleaning Launcher ===");
	DelayedFncLauncher* launcher = (DelayedFncLauncher*) arg;

	logger->inf(LOG_TIMER, "CLEAN: Ask-LOCK");
	logger->inf(LOG_TIMER, "CLEAN: TEST Ask-LOCK: %d", launcher->id);
	LOCK(launcher);
	logger->inf(LOG_TIMER, "CLEAN: LOCK");

	logger->inf(LOG_TIMER, "-- Is Killed: %d", launcher->killed);

	launcher->completed = !launcher->killed;


	if (launcher->fnc_thread != NULL) {
		UNLOCK(launcher);
		logger->inf(LOG_TIMER, "CLEAN: UN-LOCK");

		logger->inf(LOG_TIMER, "-- Killing Function Thread");
		pthread_kill(*(launcher->fnc_thread), 1);
		pthread_t* th = &(launcher->thread);

		logger->inf(LOG_TIMER, "-- JOINING FUNCTION THREAD");
		pthread_join(*th, NULL);
		logger->inf(LOG_TIMER, "-- FUNCTION THREAD JOINED !!!");

		return;
	}
	else if(launcher->killed) {
		logger->inf(LOG_TIMER, "-- Can't Kill Function, Thread Is NULL");
	}

	UNLOCK(launcher);
	logger->inf(LOG_TIMER, "CLEAN: UN-LOCK");
}

void* callDelayedFunction(void* param) {
	logger->inf(LOG_TIMER, "=== CALLING DELAYED FUNCTION ===");


	// ListManager* launchers = getLaunchers();
	ListManager* delayedFncs = getDelayedFunctions();
    DelayedFunction* fncParam = (DelayedFunction*) param;
    DelayedFncLauncher* launcher = fncParam->launcher;

	logger->inf(LOG_TIMER, "-- Prepare Launcher CallBack");
	pthread_cleanup_push(cleanLauncher, (void*) launcher);

	Project* pro = getProject();
	pthread_t* th = &(fncParam->thread);

	LOCK(fncParam);
	while (!fncParam->doBreak && pro->status != PRO_CLOSE) {
		if (fncParam->delay > 0) {

			logger->inf(LOG_TIMER, "-- Waiting: %f", fncParam->delay);

			WAIT_TIME(launcher, fncParam->delay);
			UNLOCK(launcher);
			//pthread_cond_timedwait(&launcher->cond, &launcher->mutex, &waitTime);
		}

		if (fncParam->doBreak) {
			logger->inf(LOG_TIMER, "-- Break Timer");
			UNLOCK(fncParam);
			break;
		}
		else if (launcher->paused) {
			WAIT(launcher);
			UNLOCK(launcher);
		}

		logger->inf(LOG_TIMER, "-- Calling Thread");
		pthread_create(&fncParam->thread, NULL, fncParam->fnc, (void*) fncParam);

		logger->inf(LOG_TIMER, "-- Waiting Thread End");

		UNLOCK(fncParam);
		pthread_join(*th, NULL);
		LOCK(fncParam);

		logger->inf(LOG_TIMER, "-- Do Break Result: %d", fncParam->doBreak);

		if (!fncParam->loop || fncParam->doBreak) {
			logger->inf(LOG_TIMER, "-- Break Timer");
			break;
		}
	}

	logger->inf(LOG_TIMER, "-- Loop Ended");

	if (fncParam->callback != NULL) {
		logger->inf(LOG_TIMER, "-- Calling CallBack");
		UNLOCK(fncParam);
		fncParam->callback(fncParam);
		LOCK(fncParam);
	}

	logger->inf(LOG_TIMER, "-- Set Completed");
	fncParam->completed = !fncParam->killed;

	logger->inf(LOG_TIMER, "-- Close Args");
	va_end(fncParam->args);

	logger->inf(LOG_TIMER, "-- REMOVE DELAYED NODE");
	removeNode(delayedFncs, fncParam->node);

	logger->inf(LOG_TIMER, "-- FREE PARAM NAME");
	free(fncParam->name);

	logger->inf(LOG_TIMER, "-- FREE PARAM");
	UNLOCK(fncParam);
	free(fncParam);

	logger->inf(LOG_TIMER, "-- CALL: Ask-Lock");
	LOCK(launcher);
	logger->inf(LOG_TIMER, "-- CALL: Lock");
	logger->inf(LOG_TIMER, "-- Clean Launcher Pointers");

	launcher->fnc = NULL;
	launcher->fnc_node = NULL;
	launcher->completed = true;
	launcher->fnc_thread = NULL;
	UNLOCK(launcher);

	logger->inf(LOG_TIMER, "-- CALL: UN-Lock");

	logger->inf(LOG_TIMER, "-- Delayed Done !!!!");
	pthread_cleanup_pop(1);

	return NULL;
}

DelayedFncLauncher* delayed(double delay, bool loop, void* (*fnc)(void*), void* (*callback)(DelayedFunction*), ...) {
	static int id = 0;
	logger->inf(LOG_TIMER, "=== NEW DELAYED FUNCTION ===");

	ListManager* delayedFncs = getDelayedFunctions();

	char name[150];
	memset(name, 0, 150);
	snprintf(name, 150, "delayedFnc-%d", id++);
	logger->inf(LOG_TIMER, "-- New Thread: %s", name);


	logger->inf(LOG_TIMER, "-- Prepare Arguments");
	va_list args;
	va_start(args, callback);

	logger->inf(LOG_TIMER, "-- Prepare Structure");
	DelayedFunction* fncParam = new(DelayedFunction);
	Node* n = addNodeV(delayedFncs, name, fncParam, 1);

	fncParam->id = n->id;
	fncParam->fnc = fnc;
	fncParam->loop = loop;
	fncParam->args = args;
	fncParam->killed = false;
	fncParam->delay = delay;
	fncParam->doBreak = false;
	fncParam->name = Str(name);
	fncParam->completed = false;
	fncParam->callback = callback;

	//va_end(va);

	logger->inf(LOG_TIMER, "-- New Thread ID: %d", n->id);
	fncParam->node = n;


	logger->inf(LOG_TIMER, "-- STORING LAUNCHER");

	ListManager* launchers = getLaunchers();
	DelayedFncLauncher* launcher = new(DelayedFncLauncher);
	launcher->node = fncParam->timer_node = addNodeV(launchers, "Timer Thread", launcher, 0);

	launcher->id = n->id;
	launcher->fnc_node = n;
	launcher->fnc = fncParam;
	launcher->killed = false;
	launcher->paused = false;
	launcher->completed = false;
	launcher->name = Str(fncParam->name);
	launcher->fnc_thread = &fncParam->thread;
	pthread_t* tmpThread = &(launcher->thread);


	fncParam->launcher = launcher;
	fncParam->timer_thread = tmpThread;


	logger->inf(LOG_TIMER, "-- Create Call Thread");
	pthread_create(tmpThread, NULL, callDelayedFunction, (void*) fncParam);


	return launcher;
}

short clearDelayed(int i, Node* n, short* delete, void* param, va_list* args) {
	logger->inf(LOG_TIMER, "=== Clearing DELAYED: %s ===", n->name);
	DelayedFncLauncher* launcher = n->value;

	if (!launcher->killed && !launcher->completed) {
		logger->war(LOG_TIMER, "=== Clearing Un Completed DELAYED Function: %s ===", n->name);
		killDelayedFunction(launcher, true, true);
	}

	free(launcher->name);
	pthread_t* th = &launcher->thread;
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


bool killDelayedFunction(DelayedFncLauncher* launcher, bool force, bool join) {
	logger->war(LOG_TIMER, "##### Killing Delayed Function #####");
	if (launcher == NULL) {
		logger->war(LOG_TIMER, "=== Trying To Kill A Delayed Function But Launcher Pointer Is Null.");
		return false;
	}

	logger->inf(LOG_TIMER, "KILL: Ask-LOCK");
	LOCK(launcher);
	logger->inf(LOG_TIMER, "KILL: LOCK");

	launcher->killed = true;

	if (launcher->fnc != NULL) {
        launcher->fnc->killed = true;
        launcher->fnc->doBreak = true;
		logger->war(LOG_TIMER, "=== Breaking Function");
	}
	else {
		logger->war(LOG_TIMER, "=== Trying To Kill A Delayed Function But Launcher->fnc Pointer Is Null.");

	}

	pthread_t th = launcher->thread;
	UNLOCK(launcher);
	logger->inf(LOG_TIMER, "KILL: UN-LOCK");

	if (force) {
		logger->inf(LOG_TIMER, "-- FORCE CONDITION KILL FUNCTION THREAD");
		SIGNAL(launcher);

		logger->inf(LOG_TIMER, "-- FORCE KILL FUNCTION THREAD");
		pthread_kill(th, 1);

		logger->inf(LOG_TIMER, "-- FORCE KILL FUNCTION THREAD CALLED");
	}


	if (join) {
		logger->inf(LOG_TIMER, "-- KILL JOINING LAUNCHER THREAD");
		pthread_join(th, NULL);
		logger->inf(LOG_TIMER, "-- KILL LAUNCHER THREAD JOINED !!!");
	}

	logger->inf(LOG_TIMER, "-- KILL LAUNCHER THREAD SUCCESS !!!");
	return true;
}

bool pauseDelayedFunction(DelayedFncLauncher* launcher) {
	if (launcher == NULL) {
		logger->war(LOG_TIMER, "=== Trying To Pause A Delayed Function But Launcher Pointer Is Null.");
		return false;
	}

	logger->inf(LOG_TIMER, "=== PAUSING DELAYED: %s ===", launcher->name);
	LOCK(launcher);
	launcher->paused = true;
	UNLOCK(launcher);

	return true;
}

bool resumeDelayedFunction(DelayedFncLauncher* launcher) {
	if (launcher == NULL) {
		logger->war(LOG_TIMER, "=== Trying To Resume A Delayed Function But Launcher Pointer Is Null.");
		return false;
	}

	if (!launcher->paused) {
		return true;
	}

	logger->inf(LOG_TIMER, "=== RESUMING DELAYED: %s ===", launcher->name);
	launcher->paused = false;
	SIGNAL(launcher);

	return true;
}
