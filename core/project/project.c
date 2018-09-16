#include "project.h"
#include "../../base/logger.h"
#include "../../base/json.h"
#include "../object/object.h"
#include "../asset/asset.h"
#include "../timer/timer.h"
#include "../event/control/control.h"

void addDebugFlag(char* flag) {
	Project* pro = getProject();

	logger->inf(LOG_PROJECT, "==== setting Flag: %s ====", flag);
	Node* n = getNodeByName(pro->flagList, flag);

	if(n == NULL) {
		logger->err(LOG_PROJECT, "Faild To Found Flag: %s", flag);
		return;
	}

	enableLoggerTag(*((int*)n->value));
	pro->flags = pro->flags | *((int*) n->value);
}


void initProjectFlags(Project* pro) {
	pro->flagList = initListMgr();

	static unsigned int flags[18] = {
	    LOG_NONE,
		LOG_JSON,
		LOG_MAIN,
		LOG_OBJ,
		LOG_ASSET,
		LOG_VIEW,
		LOG_LAYER,
		LOG_EVENT,
		LOG_COMMON,
		LOG_PROJECT,
		LOG_RENDER,
		LOG_ANIM,
		LOG_TIMER,
		LOG_SPRITE,
		LOG_CHAR,
		LOG_CONTROL,
		LOG_CONTROLER,
	};


	addNodeV(pro->flagList, "none", &flags[0], 0);
	addLoggerTag(flags[0], "none", 0);

	addNodeV(pro->flagList, "json", &flags[1], 0);
	addLoggerTag(flags[1], "json", 0);

	addNodeV(pro->flagList, "main", &flags[2], 0);
	addLoggerTag(flags[2], "main", 0);

	addNodeV(pro->flagList, "obj", &flags[3], 0);
	addLoggerTag(flags[3], "obj", 0);

	addNodeV(pro->flagList, "asset", &flags[4], 0);
	addLoggerTag(flags[4], "asset", 0);

	addNodeV(pro->flagList, "view", &flags[5], 0);
	addLoggerTag(flags[5], "view", 0);

	addNodeV(pro->flagList, "layer", &flags[6], 0);
	addLoggerTag(flags[6], "layer", 0);

	addNodeV(pro->flagList, "event", &flags[7], 0);
	addLoggerTag(flags[7], "event", 0);

	addNodeV(pro->flagList, "common", &flags[8], 0);
	addLoggerTag(flags[8], "common", 0);

	addNodeV(pro->flagList, "project", &flags[9], 0);
	addLoggerTag(flags[9], "project", 0);

	addNodeV(pro->flagList, "render", &flags[10], 0);
	addLoggerTag(flags[10], "render", 0);

	addNodeV(pro->flagList, "anim", &flags[11], 0);
	addLoggerTag(flags[11], "anim", 0);

	addNodeV(pro->flagList, "timer", &flags[12], 0);
	addLoggerTag(flags[12], "timer", 0);

	addNodeV(pro->flagList, "sprite", &flags[13], 0);
	addLoggerTag(flags[13], "sprite", 0);

	addNodeV(pro->flagList, "char", &flags[14], 0);
	addLoggerTag(flags[14], "char", 0);

	addNodeV(pro->flagList, "control", &flags[15], 0);
	addLoggerTag(flags[15], "control", 0);

	addNodeV(pro->flagList, "control", &flags[16], 0);
	addLoggerTag(flags[16], "controller", 0);
}


Project* getProject() {
	static Project* project = NULL;

	if (project != NULL){
		return project;
	}

	project = new(Project);
	return project;
}


void parseProjectArgs(int argc, char* argv[]) {
	static Arg arg1 = {
		.name = "-dbg",
		.function = addDebugFlag,
		.hasParam = 1,
		.defParam = NULL,
		.asInt = 0,
		.type="alpha"
	};

	static  Arg* args[] = {
		&arg1,
		NULL
	};

	ListManager* lst = defineArgs(args);
	parseArgs(lst, argc, argv);
	deleteList(lst);
}

Project* initProject(int argc, char* argv[]) {
	logger->inf(LOG_PROJECT, "==== INIT PROJECT ====");
	Project* pro = getProject();

	pro->status = PRO_INIT;
	logger->inf(LOG_PROJECT, "==== INIT PROJECT FLAGS ====");
	initProjectFlags(pro);
	
	logger->inf(LOG_PROJECT, "==== INIT PROJECT ARGS ====");
	parseProjectArgs(argc, argv);

	return pro;
}

void changeStatus(ProjectState state) {
	logger->inf(LOG_PROJECT, "##### CHANGING STATUS: %s #####", GEN_STATE_STRING[state]);
	Project* pro = getProject();
	logger->inf(LOG_PROJECT, "GOT PROJECT");

	logger->inf(LOG_PROJECT, "-- Project: ASK-LOCK");
	LOCK(pro);
	logger->inf(LOG_PROJECT, "-- Project: LOCKED");


	pro->status = state;
	logger->inf(LOG_PROJECT, "STATE CHANGED");


	UNLOCK(pro);
	logger->inf(LOG_PROJECT, "-- Project: UN-LOCK");
}

void closeProject() {
	logger->inf(LOG_PROJECT, "==== Closing Project ====");
	Project* pro = getProject();

	logger->inf(LOG_PROJECT, "-- Clear Timers");
	clearDelayedFunctions();

	logger->inf(LOG_PROJECT, "-- Clear Objects");
	clearObjects();

	ListManager* objects = getObjectList();
	if (objects != NULL) {
		deleteList(objects);
	}

	AssetMgr* ast = getAssets();
	ast->destroy();

	logger->dbg(LOG_PROJECT, "-- Free Project");
	deleteList(pro->flagList);
	pro->status = PRO_CLOSE;
	free(pro);

	logger->dbg(LOG_PROJECT, "==== Killing Project Proccess ====");
}
