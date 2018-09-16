#include "controller.h"

ListManager* getPlayerList() {
	static ListManager* players = NULL;

	if (players != NULL) {
		return players;
	}

	players = initListMgr();
	return players;
}

bool move(Controller* ctrl, SDL_Rect pos) {
	if (!ctrl->enabled || ctrl->character == NULL) {
		return false;
	}

	return true;
}

void ctrlSetCharacter(Controller* ctrl, Character* c) {
	if (ctrl->character != NULL) {
		ctrl->removeCharacter(ctrl);
	}

	ctrl->character = c;
}

void ctrlRemoveCharacter(Controller* ctrl) {
	characterDelete(ctrl->character);
	ctrl->character = NULL;
}

void initController(Controller* ctrl, ControllerType type) {
	ctrl->type = type;
	ctrl->enabled = true;

	ctrl->move = move;
	ctrl->moveToObj = NULL;
	ctrl->setCharacter = ctrlSetCharacter;
	ctrl->removeCharacter = ctrlRemoveCharacter;
}
