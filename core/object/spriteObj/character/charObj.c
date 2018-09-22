#include "charObj.h"
#include "../../../../common.h"

CharObj* newCharObj(char* name, char* charName, SDL_Rect* pos, short z, CharacterType type, Character* ch) {
	logger->inf(LOG_CHAR, "-- New Char Obj: %s", name);

	CharObj* obj = (CharObj*) initSpriteObj((SpriteObject*) new(CharObj), name, charName, pos, z);
	bool b = LOCK(obj, "NEW CHAR-0");


	logger->dbg(LOG_CHAR, "-- INIT SpriteObj: %s", name);

	logger->dbg(LOG_CHAR, "-- Setting Controller Obj: %s", name);
	obj->ch = ch;
	if (ch == NULL) {
		obj->ctrl = NULL;
	}
	else{
		obj->ctrl = ch->ctrl;
	}

	logger->dbg(LOG_CHAR, "-- Char Obj Ready: %s", name);

	UNLOCK(obj, "NEW CHAR-1", b);
	return obj;
}
