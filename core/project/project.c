#include "project.h"
#include "../object/object.h"
#include "../asset/asset.h"

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

	static unsigned int flags[11] = {
	    LOG_NONE,
		LOG_MAIN,
		LOG_OBJ,
		LOG_ASSET,
		LOG_VIEW,
		LOG_LAYER,
		LOG_EVENT,
		LOG_COMMON,
		LOG_PROJECT,
		LOG_RENDER
	};


	addNodeV(pro->flagList, "none", &flags[0], 0);
	addLoggerTag(flags[0], "none", 0);

	addNodeV(pro->flagList, "main", &flags[1], 0);
	addLoggerTag(flags[1], "main", 0);

	addNodeV(pro->flagList, "obj", &flags[2], 0);
	addLoggerTag(flags[2], "obj", 0);

	addNodeV(pro->flagList, "asset", &flags[3], 0);
	addLoggerTag(flags[3], "asset", 0);

	addNodeV(pro->flagList, "view", &flags[4], 0);
	addLoggerTag(flags[4], "view", 0);

	addNodeV(pro->flagList, "layer", &flags[5], 0);
	addLoggerTag(flags[5], "layer", 0);

	addNodeV(pro->flagList, "event", &flags[6], 0);
	addLoggerTag(flags[6], "event", 0);

	addNodeV(pro->flagList, "common", &flags[7], 0);
	addLoggerTag(flags[7], "common", 0);

	addNodeV(pro->flagList, "project", &flags[8], 0);
	addLoggerTag(flags[8], "project", 0);

	addNodeV(pro->flagList, "render", &flags[9], 0);
	addLoggerTag(flags[9], "render", 0);
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
	Project* pro = getProject();

	pro->status = PRO_INIT;
	initProjectFlags(pro);
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