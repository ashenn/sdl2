#ifndef VIEW_H
#define VIEW_H

#include<SDL2\SDL.h>
#include "../../common.h"
#include "../../base/libList.h"
#include "../object/object.h"
#include "./layer/layer.h"

SDL_Window* getWindow();
//void renderObjects();
SDL_Renderer* getRenderer();
void printObject(Object* obj);
#endif