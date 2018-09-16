#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <SDL2/SDL.h>
#include "../../base/math.h"

typedef enum DirectionEnum
{
	DIR_TOP,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT
} DirectionEnum;

vector getDirection(SDL_Rect pos, SDL_Rect target);

#endif