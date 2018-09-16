#include "control.h"


#include "../event.h"
#include "../../../base/logger.h"
#include "../../../base/json.h"
#include "../../asset/asset.h"

ListManager* getKeyList() {
	static ListManager* keys = NULL;

	if (keys != NULL) {
		return keys;
	}

	keys = initListMgr();
	return keys;
}

bool bindControl(unsigned int i, Json* conf, void* target) {
	ControlMgr* controls = getControls();
	logger->inf(LOG_CONTROL, "=== APPLY CONTROL CONF #%d: %s", i, conf->key);

	jsonPrint(conf, 0);

	logger->inf(LOG_CONTROL, "-- Getting  Key: %s", conf->key);
	SDL_Keycode key = getKey(conf->key);

	if (key == -1) {
		logger->war(LOG_CONTROL, "Fail To Bind Control: %s", conf->key);
		return true;
	}

	logger->inf(LOG_CONTROL, "-- Binding Key: %s", conf->key);

	Node* n = NULL;
	char* press = jsonGetValue(conf, "pressed", NULL);
	logger->inf(LOG_CONTROL, "-- Getting  Press: %s", press);

	KeyEvent* evt = bindKeyEvent(conf->key, key, NULL);
	logger->inf(LOG_CONTROL, "Setting Event Target");
	evt->target = target;

	if (press != NULL) {
		logger->inf(LOG_CONTROL, "-- Searching Function: %s", press);
		n = getControl(press);

		if (n != NULL) {
			logger->inf(LOG_CONTROL, "-- Assing Press: %s", press);
			evt->pressed = n->fnc;
		}
	}


	char* release = jsonGetValue(conf, "release", NULL);
	logger->inf(LOG_CONTROL, "-- Getting  Release: %s", release);

	if (release != NULL) {
		logger->inf(LOG_CONTROL, "-- Searching Function: %s", release);
		n = getControl(release);

		if (n != NULL) {
			logger->inf(LOG_CONTROL, "-- Assing Release: %s", release);
			evt->released = n->fnc;
		}
	}


	Json* hold = jsonGetData(conf, "hold");
	logger->inf(LOG_CONTROL, "-- Getting  Hold: %p", hold);

	if (hold == NULL || hold->type == JSON_NULL) {
		logger->inf(LOG_CONTROL, "-- Hold Is Null");
		return true;
	}

	logger->inf(LOG_CONTROL, "-- Getting Function");
	char* holdFnc = jsonGetValue(hold, "fnc", NULL);
	logger->inf(LOG_CONTROL, "-- Func: %s", holdFnc);

	if (holdFnc == NULL) {
		logger->war(LOG_CONTROL, "Fail To Bind Key Hold: %s, Function Is Null !!!", conf->key);
		return true;
	}

	n = getControl(holdFnc);
	if (n == NULL) {
		return true;
	}

	logger->inf(LOG_CONTROL, "-- Setting Hold Function");
	evt->hold = n->fnc;

	logger->inf(LOG_CONTROL, "-- Getting  Hold Params");
	jsonPrint(hold, 0);

	float minHold = 0;
	float maxHold = 0;
	bool callOnMax = false;

	jsonGetValue(hold, "minTime", &minHold);
	logger->inf(LOG_CONTROL, "-- MinHold: %f", minHold);
	evt->holdMin = minHold;

	jsonGetValue(hold, "maxTime", &maxHold);
	logger->inf(LOG_CONTROL, "-- MaxHold: %f", maxHold);
	evt->holdMax = maxHold;

	jsonGetValue(hold, "callOnMax", &callOnMax);
	logger->inf(LOG_CONTROL, "-- CallOnMax: %d", callOnMax);
	evt->callHoldOnMax = callOnMax;

	return true;
}

void loadControl(char* name, char* key, void* target) {
	ControlMgr* controls = getControls();
	logger->inf(LOG_CONTROL, "==== LOADING CONTROL: %s ====", name);

	logger->dbg(LOG_CONTROL, "-- Search In Ctrls: %s", key);
	Node* keyN = getNodeByName(controls->ctrls, key);
	logger->dbg(LOG_CONTROL, "-- Search Done");

	if (keyN != NULL) {
		logger->war(LOG_CONTROL, "Trying to Load Multiple times Control: %s With Key: %s", name, key);
		return;
	}

	logger->dbg(LOG_CONTROL, "-- Search In Confs: %s", key);
	Json* conf = NULL;
	Node* confN = getNodeByName(controls->confs, name);
	logger->dbg(LOG_CONTROL, "-- Search Done");

	if (confN == NULL) {
		AssetMgr* ast = getAssets();
		logger->inf(LOG_CONTROL, "-- Getting Ctrls:");
		Json* json = ast->getConf("control/control");

		if (json == NULL) {
			logger->err(LOG_CONTROL, "-- Fail To Find Conf");
			return;
		}

		logger->inf(LOG_CONTROL, "-- Controls Found !!!");
		jsonPrint(json, 0);

		logger->inf(LOG_CONTROL, "-- Getting: %s", name);
		conf = jsonGetData(json, name);
		if (conf == NULL) {
			logger->err(LOG_CONTROL, "Fail To Find Control: %s", name);
			return;
		}

		jsonPrint(conf, 0);
		confN = addNodeV(controls->confs, name, conf, 1);
	}
	else {
		logger->dbg(LOG_CONTROL, "-- Cast Conf");
		conf = (Json*) confN->value;
	}

	logger->dbg(LOG_CONTROL, "-- Getting Key: %s", key);
	Json* value = jsonGetData(conf, key);
	if (value == NULL) {
		logger->war(LOG_CONTROL, "Fail To Find Control %s With Name: %s", name, key);
		return;
	}

	logger->dbg(LOG_CONTROL, "-- Iterating Conf: %s", key);
	jsonIterate(value, bindControl, target);
}

bool addControl(char* name, void* (*fnc)(void*)) {
	logger->inf(LOG_CONTROL, "==== ADDING CONTROLS: %s ====", name);
	ControlMgr* controls = getControls();

	Node* n = getNodeByName(controls->fncs, name);
	if (n != NULL) {
		logger->war(LOG_CONTROL, "Trying to add Multiple times Control: %s", name);
		return false;
	}

	n = addNode(controls->fncs, name);
	n->fnc = fnc;
	logger->inf(LOG_CONTROL, "-- Added !!!");

	return true;
}

Node* getControl(char* name) {
	ControlMgr* controls = getControls();
	Node* n = getNodeByName(controls->fncs, name);

	if (n == NULL) {
		logger->war(LOG_CONTROL, "FAIL To Find Control Function: %s", name);
	}

	return n;
}

SDL_Keycode getKey(char* name) {
	logger->inf(LOG_CONTROL, "==== Getting Key: %s ====", name);
	ListManager* keys = getKeyList();

	Node* n = getNodeByName(keys, name);
	if (n == NULL) {
		logger->war(LOG_CONTROL, "Fail de Find Key: %s ====", name);
		return -1;
	}

	logger->inf(LOG_CONTROL, "-- Key Found");
	return *((SDL_Keycode*) n->value);
}

void initKeys() {
	static bool isInit = false;

	if (isInit) {
		return;
	}

	logger->inf(LOG_CONTROL, "==== INIT CONTROLS KEYS ====");
	ListManager* keys = getKeyList();

	logger->inf(LOG_CONTROL, "-- Adding SDLK_UP");
	static SDL_Keycode k_up = SDLK_UP;
	addNodeV(keys, "SDLK_UP", &k_up, 0);

	logger->inf(LOG_CONTROL, "-- Adding SDLK_LEFT");
	static SDL_Keycode k_left = SDLK_LEFT;
	addNodeV(keys, "SDLK_LEFT", &k_left, 0);

	logger->inf(LOG_CONTROL, "-- Adding SDLK_RIGHT");
	static SDL_Keycode k_right = SDLK_RIGHT;
	addNodeV(keys, "SDLK_RIGHT", &k_right, 0);

	logger->inf(LOG_CONTROL, "-- Adding SDLK_DOWN");
	static SDL_Keycode k_down = SDLK_DOWN;
	addNodeV(keys, "SDLK_DOWN", &k_down, 0);
}

ControlMgr* getControls() {
	static ControlMgr* controls = NULL;

	if (controls != NULL) {
		return controls;
	}

	controls = new(ControlMgr);
	controls->fncs = initListMgr();
	controls->confs = initListMgr();
	controls->ctrls = initListMgr();

	initKeys();

	return controls;
}
