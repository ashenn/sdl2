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

	//logger->err(LOG_ANIM, "Lock Launcher");
	LOCK(launcher, "CLEAN LAUNCHER-0");
	logger->inf(LOG_TIMER, "CLEAN: LOCK");

	logger->inf(LOG_TIMER, "-- Is Killed: %d", launcher->killed);

	launcher->completed = !launcher->killed;


	if (launcher->fnc_thread != NULL) {
		UNLOCK(launcher, "CLEAN LAUNCHER-1", true);
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

	//logger->err(LOG_ANIM, "UnLock Launcher");
	UNLOCK(launcher, "CLEAN LAUNCHER-2", true);
	logger->inf(LOG_TIMER, "CLEAN: UN-LOCK");
}

void* callDelayedFunction(void* param) {
	logger->inf(LOG_TIMER, "=== CALLING DELAYED FUNCTION ===");

	ListManager* delayedFncs = getDelayedFunctions();
    DelayedFunction* fncParam = (DelayedFunction*) param;
    DelayedFncLauncher* launcher = fncParam->launcher;

	logger->dbg(LOG_TIMER, "-- Prepare Launcher CallBack");
	pthread_cleanup_push(cleanLauncher, (void*) launcher);

	Project* pro = getProject();
	pthread_t* th = &(fncParam->thread);

	//logger->err(LOG_ANIM, "Lock Delayed Func");
	LOCK(launcher, "Call Delayed-0.0");
	LOCK(fncParam, "Call Delayed-0.1");
	while (!fncParam->doBreak && pro->status != PRO_CLOSE) {
		//logger->err(LOG_ANIM, "UnLock Launcher");
		UNLOCK(launcher, "Call Delayed-1", true);
		if (fncParam->delay != 0) {

			logger->dbg(LOG_TIMER, "-- Waiting: %f", fncParam->delay);
			WAIT_TIME(launcher, fncParam->delay);
			//pthread_cond_timedwait(&launcher->cond, &launcher->mutex, &waitTime);
		}

		if (fncParam->doBreak) {
			logger->dbg(LOG_TIMER, "-- Break Timer");
			break;
		}
		else if (launcher->paused) {
			launcher->pauseAt = microTime();
			WAIT(launcher);
		}

		logger->dbg(LOG_TIMER, "-- Calling Thread");
		pthread_create(&fncParam->thread, NULL, fncParam->fnc, (void*) fncParam);

		logger->dbg(LOG_TIMER, "-- Waiting Thread End");

		pthread_join(*th, NULL);
		//logger->err(LOG_ANIM, "Lock Delayed Func");
		LOCK(fncParam, "Call Delayed-2");

		logger->dbg(LOG_TIMER, "-- Do Break Result: %d", fncParam->doBreak);

		if (!fncParam->loop || fncParam->doBreak) {
			logger->dbg(LOG_TIMER, "-- Break Timer");
			break;
		}
	}

	logger->dbg(LOG_TIMER, "-- Loop Ended");

	if (fncParam->callback != NULL) {
		logger->dbg(LOG_TIMER, "-- Calling CallBack");
		//logger->err(LOG_ANIM, "UnLock Delayed Func");
		UNLOCK(fncParam, "Call Delayed-3", true);
		fncParam->callback(fncParam);
		//logger->err(LOG_ANIM, "Lock Delayed Func");
		LOCK(fncParam, "Call Delayed-4");
	}

	logger->dbg(LOG_TIMER, "-- Set Completed");
	fncParam->completed = !fncParam->killed;

	logger->dbg(LOG_TIMER, "-- Close Args");
	va_end(*(fncParam->args));

	logger->dbg(LOG_TIMER, "-- REMOVE DELAYED NODE");
	removeNode(delayedFncs, fncParam->node);

	logger->dbg(LOG_TIMER, "-- FREE PARAM NAME");
	free(fncParam->name);

	logger->dbg(LOG_TIMER, "-- FREE PARAM");
	//logger->err(LOG_ANIM, "UnLock Delayed Func");
	UNLOCK(fncParam, "Call Delayed-5", true);
	free(fncParam);

	logger->dbg(LOG_TIMER, "-- CALL: Ask-Lock");
	//logger->err(LOG_ANIM, "Lock Launcher");
	//LOCK(launcher, "Call Delayed-6");
	logger->dbg(LOG_TIMER, "-- CALL: Lock");
	logger->dbg(LOG_TIMER, "-- Clean Launcher Pointers");

	launcher->fnc = NULL;
	launcher->fnc_node = NULL;
	launcher->completed = true;
	launcher->fnc_thread = NULL;
	//logger->err(LOG_ANIM, "UnLock Launcher");
	//UNLOCK(launcher, "Call Delayed-7", true);

	logger->dbg(LOG_TIMER, "-- CALL: UN-Lock");

	logger->dbg(LOG_TIMER, "-- Delayed Done !!!!");
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


	va_start(args, callback);
	logger->inf(LOG_TIMER, "-- Prepare Structure");
	DelayedFunction* fncParam = new(DelayedFunction);
	Node* n = addNodeV(delayedFncs, name, fncParam, 1);

	fncParam->id = n->id;
	fncParam->fnc = fnc;
	fncParam->loop = loop;
	fncParam->args = &args;
	fncParam->killed = false;
	fncParam->delay = delay;
	fncParam->doBreak = false;
	fncParam->name = Str(name);
	fncParam->completed = false;
	fncParam->callback = callback;
	fncParam->param = va_arg(args, void*);

	//va_end(va);

	logger->inf(LOG_TIMER, "-- New Thread ID: %d", n->id);
	fncParam->node = n;


	logger->inf(LOG_TIMER, "-- STORING LAUNCHER");

	ListManager* launchers = getLaunchers();
	DelayedFncLauncher* launcher = new(DelayedFncLauncher);
	launcher->node = fncParam->timer_node = addNodeV(launchers, "Timer Thread", launcher, 0);

	launcher->id = n->id;
	launcher->fnc_node = n;

	launcher->startAt = 0;
	launcher->pauseAt = 0;

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
	launcher->startAt = microTime();
	pthread_create(tmpThread, NULL, callDelayedFunction, (void*) fncParam);


	return launcher;
}

short clearDelayed(int i, Node* n, short* delete, void* param, va_list* args) {
	logger->inf(LOG_TIMER, "=== Clearing DELAYED: %s ===", n->name);
	DelayedFncLauncher* launcher = n->value;

	if (!launcher->killed && !launcher->completed) {
		logger->inf(LOG_TIMER, "=== Clearing Un Completed DELAYED Function: %s ===", n->name);
		killDelayedFunction(launcher, true, false);
	}

	logger->inf(LOG_TIMER, "=== Clearing Un Completed DELAYED Function: %s ===", n->name);

	free(launcher->name);

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
	logger->inf(LOG_TIMER, "##### Killing Delayed Function #####");
	if (launcher == NULL) {
		logger->war(LOG_TIMER, "=== Trying To Kill A Delayed Function But Launcher Pointer Is Null.");
		return false;
	}

	logger->inf(LOG_TIMER, "KILL: Ask-LOCK");
	LOCK(launcher, "Kill Launcher-0");
	logger->inf(LOG_TIMER, "KILL: LOCK");

	launcher->killed = true;

	if (launcher->fnc != NULL) {
        launcher->fnc->killed = true;
        launcher->fnc->doBreak = true;
		logger->inf(LOG_TIMER, "=== Breaking Function");
	}
	else {
		logger->war(LOG_TIMER, "=== Trying To Kill A Delayed Function But Launcher->fnc Pointer Is Null.");

	}

	pthread_t th = launcher->thread;
	UNLOCK(launcher, "Kill Launcher-1", true);
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

	logger->err(LOG_TIMER, "=== PAUSING DELAYED: %s ===", launcher->name);
	LOCK(launcher, "PAUSE Launcher-0");
	launcher->paused = true;
	UNLOCK(launcher, "PAUSE Launcher-1", true);

	return true;
}

bool resumeDelayedFunction(DelayedFncLauncher* launcher) {
	if (launcher == NULL) {
		logger->war(LOG_TIMER, "=== Trying To Resume A Delayed Function But Launcher Pointer Is Null.");
		return false;
	}

	logger->inf(LOG_TIMER, "=== RESUMING DELAYED: %s ===", launcher->name);
	launcher->paused = false;
	SIGNAL(launcher);

	return true;
}
