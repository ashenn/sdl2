#ifndef CHER_OBJ_H
#define CHER_OBJ_H

#include "../spriteObj.h"
#include "../../../character/character.h"

#define CHAR_OBJ_BODY \
	SPRITE_OBJ_BODY	\
	Character* ch;	\
	Controller* ctrl;

typedef struct CharObj
{
	CHAR_OBJ_BODY
} CharObj;

CharObj* newCharObj(char* name, char* charName, SDL_Rect* pos, short z, CharacterType type, Character* ch);
#endif
