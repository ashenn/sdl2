#include "character.h"
#include "../asset/asset.h"
#include "../../base/json.h"
#include "../object/spriteObj/character/charObj.h"

Character* newCharacter() {
	Character* ch = new(Character);
	ch->obj = NULL;
	ch->ctrl = NULL;
	ch->direction.x = 0;
	ch->direction.y = 0;

	ch->onPossess = NULL;
	ch->onUnPossess = NULL;

	ch->attr.inAir = false;
	ch->attr.crouch = false;
	ch->attr.moving = false;
	ch->attr.doubleJump = false;
	ch->attr.canDoubleJump = true;
	ch->attr.hasDoubleJump = false;


	ch->attr.moveSpeed = 0;
	ch->attr.maxMoveSpeed = 60;

	return ch;
}

Character* initCharacter(CharacterType type, char* name, char* jsonKey, SDL_Rect* pos, int z) {
	logger->inf(LOG_CHAR, "=== INIT CHARACTER ===");
	char jsonPath[150];
	memset(jsonPath, 0, 150);

	switch (type) {
		case CHAR_PLAYER:
			snprintf(jsonPath, 150, "character/player");
			break;

		case CHAR_MONSTER:
			snprintf(jsonPath, 150, "character/monster");
			break;

		case CHAR_BOSS:
			snprintf(jsonPath, 150, "character/boss");
			break;
	}

	logger->inf(LOG_CHAR, "-- Path: %s", jsonPath);

	AssetMgr* ast = getAssets();
	Json* json = ast->getJson(jsonPath);

	if (json == NULL) {
		logger->err(LOG_CHAR, "Fail To Find Character Json: %s", jsonPath);
		return NULL;
	}

	logger->inf(LOG_CHAR, "-- Search: %s", jsonKey);
	Json* data = jsonGetData(json, jsonKey);

	if (data == NULL) {
		logger->err(LOG_CHAR, "-- Fail To Find %s Data In %s", jsonKey, jsonPath);
		return NULL;
	}

	char* sheet = jsonGetValue(data, "sheet", NULL);
	logger->inf(LOG_CHAR, "-- Sheet: %s", sheet);

	Character* ch = newCharacter();
	ch->obj = (Object*) newCharObj(name, sheet, pos, z, type, ch);

	return ch;
}

void characterDelete(Character* ch) {
	if (ch == NULL) {
		return;
	}

	logger->inf(LOG_CHAR, "===== DELETING CHARACTER: %s =====", ch->name);
	if (ch->obj != NULL) {
		if (ch->obj->delete != NULL) {
			ch->obj->delete(ch->obj);
			ch->obj = NULL;
		}
		else {
			//SpriteObjectDelete(ch->obj);
		}
	}
}
