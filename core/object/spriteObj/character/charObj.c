#include "charObj.h"
#include "../../../../common.h"

CharObj* newCharObj(char* name, char* charName, SDL_Rect* pos, short z, CharacterType type, Character* ch) {
	logger->inf(LOG_CHAR, "-- New Char Obj: %s", name);
	CharObj* obj = new(CharObj);

	logger->dbg(LOG_CHAR, "-- INIT SpriteObj: %s", name);
	initSpriteObj((SpriteObject*) obj, name, charName, pos, z);

	logger->dbg(LOG_CHAR, "-- Setting Controller Obj: %s", name);
	obj->ch = ch;
	if (ch == NULL) {
		obj->ctrl = NULL;
	}
	else{
		obj->ctrl = ch->ctrl;
	}

	logger->dbg(LOG_CHAR, "-- Char Obj Ready: %s", name);
	return obj;
}
