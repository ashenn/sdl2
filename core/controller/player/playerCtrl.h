#ifndef PLAYER_CTRL
#define PLAYER_CTRL

#include "../controller.h"
#include "../../../base/libList.h"

typedef struct PlayerCtrl
{
	CONTROLLER_BODY
	unsigned int playerID;
} player;

ListManager* getPlayerList();
PlayerCtrl* getPlayer(unsigned int id);
Controller* spawnPlayer(int id, char* name, char* jsonKey);

#endif