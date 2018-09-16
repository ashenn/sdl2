#include "charObj.h"
#include "../../../../common.h"

CharObj* newCharObj(char* name, char* charName, SDL_Rect* pos, short z, CharacterType type, Character* ch) {
	CharObj* obj = new(CharObj);
	initSpriteObj((SpriteObject*) obj, name, charName, pos, z);

	obj->ch = ch;
	if (ch == NULL) {
		obj->ctrl = NULL;
	}
	else{
		obj->ctrl = ch->ctrl;
	}

	return obj;
}
