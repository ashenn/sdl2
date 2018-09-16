#include "playerCtrl.h"

#include "../../../base/json.h"
#include "../../../base/logger.h"

ListManager* getPlayerList() {
	static ListManager* players = NULL;

	if (players == NULL) {
		return players;
	}

	players = initListMgr();
	return players;
}

PlayerCtrl* getPlayer(unsigned int id) {
	ListManager* players= getPlayerList();
	Node* n = getNode(id);
	
	if (n == NULL) {
		return NULL;
	}

	return (PlayerCtrl*) n->value;
}

PlayerCtrl* spawnPlayer(int id, char* name, char* jsonKey, SDL_Rect* pos, int z) {
	logger->inf(LOG_CONTROLLER, "===== SPAWNING PLAYER: #%d =====", id);

	ListManager* players= getPlayerList();
	Node* n = getPlayer(id);

	if (n != NULL) {
		logger->war(LOG_CONTROLLER, "Trying To Spawn Multiple Times Player: #%d", id);
		return NULL;
	}

	PlayerCtrl* ctrl = new(PlayerCtrl);
	initController(ctrl, CTRL_PLAYER);

	Character* ch = initCharacter(CHAR_PLAYER, jsonKey, pos, z);
	ctrl->setCharacter(ctrl, ch);

	return ctrl;
}