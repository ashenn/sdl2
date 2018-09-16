#include "movement.h"

vector getDirection(SDL_Rect pos, SDL_Rect target) {
	vector v;

	v.y = pos.y < target.y ? DIR_TOP : DIR_DOWN;
	v.x = pos.x < target.x ? DIR_RIGHT : DIR_LEFT;

	return v;
}