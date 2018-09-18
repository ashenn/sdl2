#include "playerCtrl.h"

#include "../../../base/json.h"
#include "../../../base/logger.h"

ListManager* getPlayerList() {
	static ListManager* players = NULL;

	if (players != NULL) {
		return players;
	}

	players = initListMgr();
	return players;
}

PlayerCtrl* getPlayer(unsigned int id) {
	ListManager* players= getPlayerList();
	logger->inf(LOG_CONTROLLER, "===== Getting PLAYER: #%d =====", id);
	Node* n = getNode(players, id);

	if (n == NULL) {
		logger->inf(LOG_CONTROLLER, "===== Not Found =====", id);
		return NULL;
	}

	logger->inf(LOG_CONTROLLER, "===== Found Find: %s =====", n->name);
	return (PlayerCtrl*) n->value;
}

PlayerCtrl* spawnPlayer(int id, char* name, char* jsonKey, SDL_Rect* pos, int z) {
	logger->inf(LOG_CONTROLLER, "===== SPAWNING PLAYER: #%d =====", id);

	PlayerCtrl* n = getPlayer(id);

	if (n != NULL) {
		logger->war(LOG_CONTROLLER, "Trying To Spawn Multiple Times Player: #%d", id);
		return NULL;
	}

	ListManager* players= getPlayerList();
	logger->inf(LOG_CONTROLLER, "===== Instance Player Controller");
	PlayerCtrl* ctrl = new(PlayerCtrl);

	logger->inf(LOG_CONTROLLER, "===== Init Controller");
	initController((Controller*) ctrl, CTRL_PLAYER);

	logger->inf(LOG_CONTROLLER, "===== Init Character");
	Character* ch = initCharacter(CHAR_PLAYER, jsonKey, pos, z);
	
	logger->inf(LOG_CONTROLLER, "===== Set Character");
	ctrl->setCharacter((Controller*) ctrl, ch);

	char ctrlName[150];
	memset(ctrlName, 0, 150);
	snprintf(ctrlName, 150, "Player-%d", id);

	addNodeV(players,  ctrlName, ctrl, 1);

	return ctrl;
}
