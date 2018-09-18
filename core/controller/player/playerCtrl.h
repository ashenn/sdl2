#ifndef PLAYER_CTRL
#define PLAYER_CTRL

#include "../controller.h"
#include "../../../base/libList.h"

typedef struct PlayerCtrl
{
	CONTROLLER_BODY
	unsigned int playerID;
} PlayerCtrl;

ListManager* getPlayerList();
PlayerCtrl* getPlayer(unsigned int id);
PlayerCtrl* spawnPlayer(int id, char* name, char* jsonKey, SDL_Rect* pos, int z);

#endif
