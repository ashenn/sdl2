#include "controller.h"

bool move(Controller* ctrl, SDL_Rect pos) {
	if (!ctrl->enabled || ctrl->character == NULL) {
		return false;
	}

	return true;
}