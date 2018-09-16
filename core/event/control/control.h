#ifndef CONTROL_H
#define CONTROL_H

#include <SDL2/SDL.h>

#include "../../../common.h"
#include "../../../base/libList.h"

typedef struct ControlFnc {
	char* key;
	void* (*fnc)(void*);
} ControlFnc;

typedef struct ControlMgr
{
	CLASS_BODY
	ListManager* fncs;
	ListManager* confs;
	ListManager* ctrls;
} ControlMgr;

ControlMgr* getControls();
Node* getControl(char* name);
SDL_Keycode getKey(char* name);
void loadControl(char* name, char* key, void* target);

bool addControl(char* name, void* (*fnc)(void*));

#endif
